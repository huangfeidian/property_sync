# property sync


## 属性同步介绍
在游戏中，角色的属性其实就是我们常说的存档，他包含了描述一个角色自身的完整数据，例如名称、等级、血量、伤害计算相关数据、buff数据、技能数据、装备包裹数据、任务包裹数据、代币数据、交易数据等，统一以一个玩家一个doc的形式存储在数据库里。一个角色的数据除了自身数据之外，还包括与其他玩家之间的关联数据，如好友、群组、聊天、帮派、队伍等，这些数据一般都是单独存库，不放在玩家身上，以避免多份数据之间的不一致问题。这些关联数据，有些时候是提供rpc形式提供客户端的读取接口，有些时候通过按需延迟初始化的属性挂载到玩家属性身上。

属性同步则是将角色属性最新的推送到所有能看到这个角色的客户端的机制，类似于Unreal的Replication，它包括两个部分：
1. 角色A进入角色B的客户端视野之后，服务端需要将A的所有对B可见的数据打包发送给B， 这样B的客户端即可根据这份数据将玩家A渲染出来，
2. 角色B视野内的角色A的特定属性改变之后，如果这个属性A对B可见，则服务端需要将这个最新数据推送到B的客户端。例如A的名字改变之后，我们将nickname属性的最新值打包推送到B，B收到之后修改本地客户端里A玩家的billboard，显示最新的名字。



## 属性结构
一个属性的值可以分为如下四种类型：
1. 基础值类型 如int,string,float,bool
2. 数组值类型，vector<T>，这里的T也是一个属性，
3. 字典值类型， Map<K, V>, 这里的k只能是int或者string，V也是一个属性，这个map形式在属性里常常作为背包使用。
4. 结构体类型，相当于一组功能相关的属性的集合，例如名字、等级、门派这三个属性一般在同一个结构体下，用来打包同步。
下面是一些属性值类型定义的例子：
```c++
struct base_info
{
    string id;// 玩家id
    uint32_t level,
    string nickname;
    uint32_t school;
}

struct buff_item
{
    float expire_ts;//buff失效时间
    uint32_t no;//buff编号
    uint32_t level;//buff等级
    uint32_t layer;//buff层数
}

using buff_data = unordered_map<uint32_t, buff_item>;//所有buff的背包

struct login_item
{
    float login_ts;//登录时间
    float logout_ts;//登出时间
}

using login_data = vector<login_item>;//记录玩家所有登录登出的时间

struct property
{
    base_info base;
    buff_data buffs;
    login_data login_record;
}
```

属性定义里面，除了他的值类型之外，还会包括另外的两个信息，同步类型和生命周期：
1. 属性的同步类型，即这个属性的值对于哪些客户端可见，可选的有三种
    1. 只存储在服务端 不对客户端进行同步
    2. 只对主角自己控制的客户端进行同步
    3. 对所有能看到自己的客户端进行同步

2. 属性的生命周期，即这个属性是否需要存库，可选的值有四种：
    1. 只在当前场景内生效，不存库，每次切换场景时设置为默认值
    2. 只在当前进程内生效，不存库，每次切换进程时设置为默认值
    3. 只在当前登陆session期间生效，不存库，每次重新登陆时设置为默认值
    4. 存库，每次玩家上线时都需要从数据库中的属性记录里拉取上次存库记录，并根据这个存库记录去初始化

综上，一个属性字段的定义，总共包括四个部分：属性名、属性值类型、属性同步类型、属性生命周期。后面的两个信息，既可以通过类继承的方式去实现，也可以通过meta的annotate标注形式去实现。为了简化讨论属性同步，我们这里将忽略这两个额外信息，不去考虑存库相关和aoi内广播的细节，只抽象为一个广播队列，对于值的推送都简化为往队列末尾加数据。同时对于属性数据的全量打包和解析相关也不讨论，因为这个就是一个对象的encode和decode的问题，在之前的文章中已经解决了。所以本篇文章所介绍的属性同步其实就是一个属性修改同步。对于这个属性修改同步，我们分别根据属性的值类型来循序渐进的来讨论同步方案。

## 简单值属性的访问控制
为了提供对属性的修改同步和增量存库功能，我们首先需要hook住对这个属性的修改操作，即为这个属性增加setter方法。如果数量不多的话，我们可以通过匿名结构体来实现：
```c++
struct Foo
{
    class {
        int value;
        public:
            int & operator = (const int &i) { return value = i; }
            operator int () const { return value; }
    } alpha;

    class {
        float value;
        public:
            float & operator = (const float &f) { return value = f; }
            operator float () const { return value; }
    } bravo;
};
```
这个匿名结构体内部重载了赋值函数和类型转换函数，对外的表现与原始类型一致。除了需要手写之外没有任何缺点，不过这一点也可以用宏来搞定。
但是很多时候，我们对于属性的getter/setter方法并不是简单的赋值而已，还有很多定制化的操作。这些操作函数一般都写在类定义里面，用来获取类里面的其他变量，此时上面的匿名类就行不通了，因为他无法访问所属类的其他变量。
对于msvc来说，这个功能已经有相应的扩展`__declspec(property)`：
```c++
// declspec_property.cpp  
struct S {  
   int i;  
   void putprop(int j) {   
      i = j;  
   }  

   int getprop() {  
      return i;  
   }  

   __declspec(property(get = getprop, put = putprop)) int the_prop;  
};  

int main() {  
   S s;  
   s.the_prop = 5;  
   return s.the_prop;  
}
```
作为功能兼容的集大成者，clang也支持这个用法,但是gcc就崩了。为了维持全平台可用这个崇高的c++愿景，我们又不得不求助于宏。
```c++
#define PROPERTY_GEN(Class, Type, Name, GetMethod, SetMethod) \
    class Property_##Name { \
    public: \
        Property_##Name(Class* parent) : _parent(parent) { } \
        Type operator = (Type value) \
        { \
            _parent->SetMethod(value); \
            return _parent->GetMethod(); \
        } \
        operator Type() const \
        { \
            return static_cast<const Class*>(_parent)->GetMethod(); \
        } \
        Property_##Name& operator =(const Property_##Name& other) \
        { \
            operator=(other._parent->GetMethod()); return *this; \
        }; \
        Property_##Name(const Property_##Name& other) = delete; \
    private: \
        Class* _parent; \
    } Name { this };


    // PROPERTY - Declares a property with the default getter/setter method names.
    #define PROPERTY(Class, Type, Name) \
        PROPERTY_GEN(Class, Type, Name, get_##Name, set_##Name)

class SomeClass
{
public:
    PROPERTY(SomeClass, int, Value)
    int get_Value() const { return _value; }
    void set_Value(int value) { _value = value; }

private:
    int _value = 0;
};


int main()
{
    SomeClass s, c;
    s.Value = 5;
    c.Value = 3 * s.Value;
    s.Value = c.Value;
}
```
这个宏也定义了一个类型来封装一个属性的getter/setter，比之前的匿名类的优势就是他能传入已经定义好的类函数，这样逻辑就更紧凑了。
modern c++的使用者们对于宏是深恶痛绝，本着能用模板就不用宏的原则，我们这里也提供一个模板的实现：
```c++
template<typename C, typename T, T (C::*getter)(), void (C::*setter)(const T&)>
struct Property
{
    C *instance;

    Property(C *instance)
        : instance(instance)
    {
    }

    operator T () const
    {
        return (instance->*getter)();
    }

    Property& operator=(const T& value)
    {
        (instance->*setter)(value);
        return *this;
    }

    template<typename C2, typename T2,
             T2 (C2::*getter2)(), void (C2::*setter2)(const T2&)>
    Property& operator=(const Property<C2, T2, getter2, setter2>& other)
    {
        return *this = (other.instance->*getter2)();
    }

    Property& operator=(const Property& other)
    {
        return *this = (other.instance->*getter)();
    }
};

struct Foo
{
    int x_, y_;

    void setX(const int& x) { x_ = x; std::cout << "x new value is " << x << "\n"; }
    int getX() { std::cout << "reading x_\n"; return x_; }

    void setY(const int& y) { y_ = y; std::cout << "y new value is " << y << "\n"; }
    int getY() { std::cout << "reading y_\n"; return y_; }

    Property<Foo, int, &Foo::getX, &Foo::setX> x;
    Property<Foo, int, &Foo::getY, &Foo::setY> y;

    Foo(int x0, int y0)
        : x_(x0), y_(y0), x(this), y(this)
    {
    }
};
```
## 简单值属性的修改同步
在上面我们实现了属性的访问控制之后，下一个需要考虑的问题就是如果把属性修改的信息通知给客户端。这个过程包括三个步骤：
1. 把修改信息以特定格式进行打包，
2. 将打包后的信息发送给客户端
3. 客户端解析数据，进行修改回放，从而实现数据一致
第二步我们可以直接忽略，因为当前文章不涉及到网络。所以我们需要处理的问题就是如何构造cmd_buffer和如何回放cmd_buffer，这里我们复用上面的`Foo`。
```c++

struct Foo
{
    int x_, y_;

    void setX(const int& x) { x_ = x; std::cout << "x new value is " << x << "\n"; }
    int getX() { std::cout << "reading x_\n"; return x_; }

    void setY(const int& y) { y_ = y; std::cout << "y new value is " << y << "\n"; }
    int getY() { std::cout << "reading y_\n"; return y_; }

    Property<Foo, int, &Foo::getX, &Foo::setX> x;
    Property<Foo, int, &Foo::getY, &Foo::setY> y;

    Foo(int x0, int y0)
        : x_(x0), y_(y0), x(this), y(this)
    {
    }
};
```
首先需要做改动的是两个set函数，不能返回void，而要返回这次修改的信息。这个信息包括两个部分：修改的变量名字和新的值：

```c++
pair<string, json> setX(const int& x)
{
    x_ = x;
    return make_pair("x", encode(x));
}
pair<string, json> setY(const int& y)
{
    y_ = y;
    return make_pair("y", encode(y));
}
```
但是业务逻辑并不关心这个修改信息的打包然后自动广播的过程，所以我们需要在类里面定义一个cmd_queue来存储这些修改信息:
```c++
queue<pair<string, json>> _cmds;
void setX(const int& x)
{
    x_ = x;
    _cmds.emplace_back("x", x);
}
void setY(const int& y)
{
    y_ = y;
    _cmds.emplace_back("y", y);
}
```
在每次全局定期tick_update函数里,等服务端处理完所有rpc和timer之后，在把queue里的信息取出来进行发送。发送到客户端之后，我们提供一个replay函数来回放相关修改：
```c++
bool replay(pair<string, json> one_cmd)
{
    const auto& [key, value] = one_cmd;
    if(key == "x")
    {
        int temp_v;
        if(!decode(value, temp_v))
        {
            return false;
        }
        _x = temp_v;
        return true;
    }
    else if(key == "y")
    {
        int temp_v;
        if(!decode(value, temp_v))
        {
            return false;
        }
        _x = temp_v;
        return true;
    }
    else
    {
        return false;
    }
}
```
基于字符串的比较其实是一个比较耗时的操作，如果我们可以将每个变量与int做一个映射，则可以调用switch case的形式来做查询优化：
```c++
static int index_for_x = 0;
static int index_for_y = 1;
queue<pair<int, json>> _cmds;

void setX(const int& x)
{
    x_ = x;
    _cmds.emplace_back(index_for_x, x);
}
void setY(const int& y)
{
    y_ = y;
    _cmds.emplace_back(index_for_y, y);
}

bool replay(pair<int, json> one_cmd)
{
    const auto& [key, value] = one_cmd;
    switch(key)
    {
    case index_for_x:
    {
        int temp_v;
        if(!decode(value, temp_v))
        {
            return false;
        }
        _x = temp_v;
        return true;
    }
    case index_for_y:
    {
        int temp_v;
        if(!decode(value, temp_v))
        {
            return false;
        }
        _y = temp_v;
        return true;
    }
    default:
        return false;
    }
    
}
```
从上面的代码可以观察出这段代码其实重复程度很高，非常适合自动代码生成，可以利用我们的meta库来生成这些重复代码。
至此只有一层深度的简单变量的修改同步已经解决了。
## 简单容器属性的修改同步
这里的简单容器属性包括vector和map，这两个容器在之前的简单值属性增加了部分更新的接口。部分更新的时候我们只需要将改变的部分同步下去即可，没有必要做完整容器的全量同步。所以我们需要在打包信息里面增加一个字段，代表属性改变方式，然后打包的value里面就只需要加改变的部分就可以了。
```c++
enum mutate_cmd
{
    clear = 0,
    set,
    vector_push,
    vector_pop,
    vector_mutate_item,
    vector_del_item,
    add,
    map_pop,
}
queue<tuple<int, mutate_cmd, json>> _cmds;
int _x;
vector<int> _y;
unordere_map<int, int> _z;

int index_for_x = 0;
int index_for_y = 1;
int index_for_z = 2;
void set_x(const int& x)
{
    _x = x;
    _cmds.emplace_back(mutate_cmd::set, index_for_x, encode(x));
}
void push_y(const int& y)
{
    _y.push_back(y);
    _cmds.emplace_back(mutate_cmd::vector_push, index_for_y, encode(y));
}
void pop_y()
{
    // something
}
void set_y(const vector<int>& value)
{
    // something
}
void clear_y()
{
    //something
}
void insert_z(const int& k, const int& v)
{
    _z[k] = v;
    _cmd.emplace_back(mutate_cmd::add, index_for_z, encode_multi(k, v));
}

void pop_z(const int& key)
{
    //something
}
void set_z(const unordered_map<int, int>& value)
{
    //something
}
void clear_z()
{
    //something
}
```
根据上述修改属性的接口，对应的replay函数定义见下：
```c++
bool replay(const tuple<int, mutate_cmd, json>& one_cmd)
{
    const auto& [index, cmd, value] = one_cmd;
    switch(index)
    {
    case index_for_x:
    {
        switch(cmd)
        {
        case mutate_cmd::set:
        {
            int temp_v;
            if(decode(value, temp_v))
            {
                _x = temp_v;
                return true;
            }
            else
            {
                return false;
            }
        }
        default
        {
            return false;
        }
        }
    }
    case index_for_y:
    {
         case mutate_cmd::vector_push:
        {
            int temp_v;
            if(decode(value, temp_v))
            {
                _y.push_back(temp_v);
                return true;
            }
            else
            {
                return false;
            }
        }
        default
        {
            return false;
        }
    }
    case index_for_z:
    {
        //some code for z
    }
    default:
        return false;
    }
}
```
上述代码基本实现了容器属性的修改同步骨架，更多的类似容器和接口只需要照着这个骨架扩充即可。但是上述简单实现有一个代码膨胀的问题，对于每个容器属性成员，我们都会生成所有的基本一样的接口函数。虽然有自动化代码生成工具来避免人工复制带来的错误，但是对于编译时间和执行文件大小来说，这种代码膨胀是不可忍受的。
## 构造proxy对象实现属性同步
为此解决代码膨胀的问题，我们修改了一下getter/setter/replay的实现方式，返回一个代理对象,来处理修改同步的问题：
```c++
template <typename T, typename B = void>
	class prop_proxy;
	template <typename T>
    class prop_proxy<T, std::enable_if_t<
		std::is_pod_v<T> || std::is_same_v<T, std::string>, void>
	>
    {
    public:
        prop_proxy(T& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& in_offset):
        _data(_in_data),
		_msg_queue(_in_msg_queue),
		_offset(in_offset)

        {

        }
		T& get_proxy_data()
		{
			return _data;
		}
		operator const T&() const
		{
			return _data;
		}
		void set(const T& _in_data)
		{
			_data = _in_data;
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::set, encode(_data));
			}
			
		}
		
		void clear()
		{
			_data = {};
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::clear, json());
			}
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data = {};
			return true;
		}
    private:
        T& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type& _offset;
	};
```
上述代码就是简单属性的代理对象，封装了修改操作和同步操作。对于访问的时候，我们需要构造一个proxy对象进行返回：
```c++
template <typename T>
prop_proxy<T, void> make_proxy(T& _in_data,
    msg_queue_base& _in_msg_queue,
    const var_idx_type& _in_offset)
{
    return prop_proxy<T, void>(_in_data, _in_msg_queue, _in_offset);
}
class base
{
    int _x;
    const decltype(_x)& x() const
	{
		return _x;
	}
	prop_proxy<decltype(_x)> x_mut()
	{
		return make_proxy(_x, _cmd_buffer, index_for_x);
	}
}
```
这两个函数代替了我们原来的setter/getter函数的功能，但是就失去了自定义setter里的检查。不过这个可以通过传递额外的setter函数到proxy对象里，或者额外提供修改接口并在接口内部处理检查之后构造proxy对象进行修改同步，所以这个自定义setter的需求也可以当作解决了，后续不再讨论。
再重新构造属性同步的骨架之后，我们再来解决通用容器接口的代码膨胀问题，这里只贴一下vector类型的proxy：
```c++
template<typename T>
	class prop_proxy<std::vector<T>>
	{
	public:
		prop_proxy(std::vector<T>& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& _in_offset) :
			_data(_in_data),
			_msg_queue(_in_msg_queue),
			_offset(_in_offset)
		{

		}
		std::vector<T>& get()
		{
			return _data;
		}
		operator const std::vector<T>&() const
		{
			return _data;
		}
		void set(const std::vector<T>& _in_data)
		{
			_data = _in_data;
			_msg_queue.add(_offset, var_mutate_cmd::set, encode(_data));
		}
		
		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}
		
		void push_back(const T& _new_data)
		{
			_data.push_back(_new_data);
			_msg_queue.add(_offset, var_mutate_cmd::add, encode(_new_data));
		}
		
		void pop_back()
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			_msg_queue.add(_offset, var_mutate_cmd::erase, json());
		}
		
		void idx_mutate(std::size_t idx, const T& _new_data)
		{
			if (idx < _data.size())
			{
				_data[idx] = _new_data;
			}
			_msg_queue.add(_offset, var_mutate_cmd::item_change, encode_multi(idx, _new_data));
		}
		
		void idx_delete(std::size_t idx)
		{
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			_msg_queue.add(_offset, var_mutate_cmd::item_change, encode(idx));
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			case var_mutate_cmd::add:
				return replay_push_back(j_data);
			case var_mutate_cmd::erase:
				return replay_pop_back(j_data);
			case var_mutate_cmd::item_change:
				return replay_idx_mutate(j_data);
			case var_mutate_cmd::erase:
				return replay_idx_delete(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_push_back(const json& j_data)
		{
			T temp;
			if (decode(j_data, temp))
			{
				_data.push_back(temp);
				return true;
			}
			else
			{
				return false;
			}
		}
		bool replay_pop_back(const json& j_data)
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			return true;
		}
		bool replay_idx_mutate(const json& j_data)
		{
			std::size_t idx;
			T temp;
			if (!decode_multi(j_data, idx, temp))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data[idx] = temp;
			}
			return true;
		}
		bool replay_idx_delete(const json& j_data)
		{
			std::size_t idx;
			if (!decode(j_data, idx))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			return true;
		}
	private:
		std::vector<T>& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};
```

## 多级属性同步
所谓的多级属性就是属性访问并不是简单x、y、z形式，而是x.y.z的形式去访问的，而x、y、z则只是多级属性里的特例，即一级属性。处理多级属性相对于一级属性来说，唯一的改变就是我们的变量名的int映射，需要改成vector<int>形式，对于x.y.z的访问来说，这个vector<int>的值包含三个元素，分别是index_for_x, index_for_y, index_for_z。
这个int改成了vector<int>之后，我们好多接口都需要重新实现一下。但是如果对属性深度设置为8和同级属性数量限定为256个以下的话，我们可以用uint64_t来编码vector<int>，这样可以维持接口的基本不变。同时int相关类型在msgpack、protobuf等打包协议里面也有很多小整数优化，单个uint64_t比vector<int>来说可能省了数十倍的空间。通过我所在的项目组来看，深度最大为8，且同级属性最多256这个限制还是非常宽裕的，目前最大深度好像是6，同级属性最大100多，主要是第一级属性。如果代码里面出现了违反这个规则的情况，预处理器直接报错提示，人工修改一下新加入的属性定义即可。如果一级属性超过了256个，可以考虑将一级属性扩充为uint16_t，后面的属性仍然保持uint8_t，这样一般来说足够用了。
多级属性同步这里的变量寻址问题除了更好的数据压缩之外，还有一个问题：我们在修改x.y.z的时候，将x, y传递到z的proxy里。当前我们构造prop_proxy只有三个参数:变量的引用，变量的类内偏移和变量的cmd_buffer。最合适的修改方法就是将cmd_buffer里带上x.y的地址信息，因为cmd_buffer是每个实例对象都不同的，而变量的类内偏移每个实例对象都一样。
```c++
using mutate_msg = std::tuple<var_prefix_idx_type, var_idx_type, var_mutate_cmd, json>;
class msg_queue_base
{
public:
    virtual void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data) = 0;
};
class msg_queue : public msg_queue_base
{
    std::deque<mutate_msg>& _queue;
    const var_prefix_idx_type& parent_idxes;
public:
    msg_queue(std::deque<mutate_msg>& _in_msg_queue,
        const var_prefix_idx_type& _in_parent_idxes)
        : _queue(_in_msg_queue)
        , parent_idxes(_in_parent_idxes)
    {

    }
    msg_queue(const msg_queue& other) = default;
    void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
    {
        _queue.emplace_back(parent_idxes, offset, _cmd, _data);
        return;
    }

};
```
为了应对这个cmd_queue的修改，我们还修改了mutate_msg的定义，从原来的三分量修改为了四分量的形式，这里的var_prefix_idx_type当前就是uint64_t，而var_idx_type则是uint8_t。
```c++
deque<mutate_msg> base_queue;
class prop_a
{
    prop_a(deque<mutate_msg>& in_queue, const var_prefix_idx_type& _in_parent_idxes)
    :_queue(in_queue), _parent_idxes(_in_parent_idxes)
    {

    }
    int _x;
    static int index_for_x = 1;
    var_prefix_idx_type _parent_idxes;
    queue<mutate_msg>& _queue;
    prop_proxy<decltype(_x)> x_mut()
	{
		return make_proxy(_x, msg_queue(_queue, _parent_idxes), index_for_x);
	}

}
class prop_b
{
     prop_b(deque<mutate_msg>& in_queue, const var_prefix_idx_type& _in_parent_idxes)
    :_queue(in_queue)
    , _parent_idxes(_in_parent_idxes)
    , a((_in_parent_idxes<<8) + index_for_a)
    {

    }
    prop_a _a;
    static int index_for_a = 1;
    var_prefix_idx_type _parent_idxes;
    queue<mutate_msg>& _queue;
    
}
```
这样，构造prop的时候一路传递deque<mutate_msg> 和_parent_idxes既可以明确改变变量时的变量地址。

## 背包类型的属性同步
背包，是属性数据结构里的核心，实现好了一个背包系统对于业务开发来说有非常大的便利，因为绝大部分的属性都是以背包形式存在的：任务背包、道具背包、装备背包、载具背包、外观背包、buff背包、技能背包等等不一而足。由于背包之间的共性很多，所以很多背包系统的代码其实都继承自同一个基类，从这个基类下面一路增添功能，在项目运营期平均两周会增加新的一种背包。下面就是一种最简状态下的背包形式：
```c++
struct common_item
{
    int item_no;
}
struct common_item_bag
{
    unordered_map<int, common_item> data;
    uint32_t capacity;
}
```
背包属性与之前介绍的`unordered_map`属性不一样的地方在于，对于`unordered_map`内的元素进行更改，只需要对这个被修改的元素进行全同步即可。而对于背包里面的item来说，这个item可能非常复杂，每次修改这个item的部分数据就来一次全量同步时非常浪费流量和cpu的。同时由于item的寻址无法被简单的编码为int形式，所以如何定义这个item的修改信息也是一个问题。此外还需要考虑item里面还有bag的问题。最后想出来的解决方案是针对背包系统，继续修改他的`cmd_buffer`：
```c++
template <typename T>
class item_msg_queue : public msg_queue_base
{
    std::deque<mutate_msg>& _queue;
    const var_prefix_idx_type& parent_idxes;
    const T& _item_key;
public:
    item_msg_queue(std::deque<mutate_msg>& _in_msg_queue,
        const var_prefix_idx_type& _in_parent_idxes,
        const T& _in_key)
        : _queue(_in_msg_queue)
        , parent_idxes(_in_parent_idxes)
        , _item_key(_in_key)
    {

    }
    item_msg_queue(const item_msg_queue& other) = default;
    void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
    {
        std::tuple<T, var_idx_type, var_mutate_cmd, json> _new_data;
        _queue.emplace_back(parent_idxes, 0, _cmd, encode_multi(_item_key, offset, _cmd, _data));
        return;
    }
};
```
更详细的背包代码就不贴了，太长了，就是根据这个`item_msg_queue`继续改相关接口就可以了。


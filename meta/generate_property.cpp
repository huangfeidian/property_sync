
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <fstream>
#include <iomanip>

#include <iostream>
#include <sstream>
#include <queue>
#include <filesystem>

#include <meta/parser/nodes/class.h>
#include <meta/parser/clang_utils.h>

//#include <meta/serialize/decode.h>
#include <meta/parser/generator.h>


using namespace std;
using namespace spiritsaway::meta;
using namespace spiritsaway::meta::generator;
json load_json_file(const std::string& filename)
{
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	auto cur_file_content = buffer.str();
	// std::cout<<"cur file_content is "<<cur_file_content<<std::endl;
	if (!json::accept(cur_file_content))
	{
		return {};
	}
	return json::parse(cur_file_content);
}
mustache::data generate_property_info_for_class(const class_node* one_class)
{
	// 生成一个类的所有property信息
	auto& the_logger = utils::get_logger();
	std::unordered_map<std::string, std::string> property_annotate_value;
	auto property_fields = one_class->query_fields_with_pred([&property_annotate_value](const variable_node& _cur_node)
		{
			return _cur_node.unqualified_name().rfind("m_", 0) != 0 && filter_with_annotation_value<variable_node>("property", property_annotate_value, _cur_node);
		});
	auto property_fields_with_base = one_class->query_fields_with_pred_recursive([&property_annotate_value](const variable_node& _cur_node)
		{
			return _cur_node.unqualified_name().rfind("m_", 0) != 0 &&  filter_with_annotation_value<variable_node>("property", property_annotate_value, _cur_node);
		});
	std::sort(property_fields.begin(), property_fields.end(), sort_by_unqualified_name<language::variable_node>);
	std::size_t field_begin_index = property_fields_with_base.size() - property_fields.size();
	std::ostringstream h_stream;
	std::ostringstream cpp_stream;
	std::string cur_class_name = one_class->qualified_name();
	auto base_classes = one_class->base_classes();
	if (base_classes.size() > 1)
	{
		the_logger.error("cant generate property for class {} with {} base classes", cur_class_name, base_classes.size());
		return "";
	}
	mustache::data render_args;
	if (base_classes.size() == 1)
	{
		render_args.set("has_base_class", true);
		render_args.set("base_class_name", base_classes[0]->unqualified_name());
		render_args.set("base_propery_idx_max", std::to_string(field_begin_index));
	}
	mustache::data field_list{ mustache::data::type::list };
	for (auto one_field : property_fields)
	{
		mustache::data cur_field_render_arg;
		auto cur_field_name = one_field->unqualified_name();
		auto cur_field_type_name = one_field->decl_type()->name();
		cur_field_render_arg.set("field_name", cur_field_name);
		cur_field_render_arg.set("field_idx", std::to_string(field_begin_index));
		field_list << cur_field_render_arg;
		field_begin_index++;
	}
	render_args.set("property_fields", field_list);
	render_args.set("class_name", one_class->unqualified_name());
	render_args.set("class_full_name", one_class->qualified_name());

	return render_args;
	

}


std::unordered_map<std::string, std::string> generate_property(const std::string& generated_folder, const std::string& mustache_folder)
{
	auto& the_logger = utils::get_logger();

	auto all_property_classes = language::type_db::instance().get_class_with_pred([](const language::class_node& _cur_node)
		{
			return language::filter_with_annotation<language::class_node>("property", _cur_node);
		});
	std::unordered_map<std::string, std::string> result;
	auto property_proxy_mustache_file = std::ifstream(mustache_folder + "property_proxy.mustache");
	std::string property_proxy_template_str = std::string(std::istreambuf_iterator<char>(property_proxy_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache property_proxy_mustache_tempalte(property_proxy_template_str);

	auto property_h_mustache_file = std::ifstream("mustache_folder + property_h.mustache");
	std::string property_h_template_str = std::string(std::istreambuf_iterator<char>(property_h_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache property_h_mustache_tempalte(property_h_template_str);

	auto property_cpp_mustache_file = std::ifstream("mustache_folder + property_cpp.mustache");
	std::string property_cpp_template_str = std::string(std::istreambuf_iterator<char>(property_cpp_mustache_file), std::istreambuf_iterator<char>());
	mustache::mustache property_cpp_mustache_tempalte(property_cpp_template_str);
	auto generated_folder_path = std::filesystem::path(generated_folder);
	for (auto one_class : all_property_classes)
	{

		auto generated_h_file_name = one_class->unqualified_name() + ".generated.h";
		auto generated_proxy_file_name = one_class->unqualified_name() + ".proxy.h";
		auto generated_cpp_file_name = one_class->unqualified_name() + ".generated.cpp";
		auto new_h_file_path = generated_folder_path / generated_h_file_name;
		auto new_proxy_file_path = generated_folder_path / generated_proxy_file_name;
		auto new_cpp_file_path = generated_folder_path / generated_cpp_file_name;

		auto render_args = generate_property_info_for_class(one_class);

		generator::append_output_to_stream(result, new_h_file_path.string(), property_proxy_mustache_tempalte.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), property_h_mustache_tempalte.render(render_args));
		generator::append_output_to_stream(result, new_h_file_path.string(), property_cpp_mustache_tempalte.render(render_args));
	}
	return result;
}
int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		std::cout << "please specify the json file path" << std::endl;
		return 1;
	}
	std::string json_file_path = argv[1];
	if (json_file_path.empty())
	{
		std::cout << "empty json file path" << std::endl;
		return 1;
	}

	auto folder_iter = json_file_path.rfind('/');
	if (folder_iter == std::string::npos)
	{
		std::cout << "file path " << json_file_path << " should has / in it " << std::endl;
		return 1;
	}
	std::string file_folder = json_file_path.substr(0, folder_iter + 1);
	auto cur_json_content = load_json_file(json_file_path);
	if (!cur_json_content)
	{
		std::cout << "empty content for json file " << json_file_path << std::endl;
		return 1;
	}
	std::vector<std::string> include_dirs;
	std::vector<std::string> compile_definitions;
	std::string src_file;
	std::string property_namespace;
	std::string mustache_folder;
	std::string generated_folder;
	try
	{
		cur_json_content.at("include_dirs").get_to(include_dirs);
		cur_json_content.at("definitions").get_to(compile_definitions);
		cur_json_content.at("src_file").get_to(src_file);
		cur_json_content.at("namespace").get_to(property_namespace);
		cur_json_content.at("mustache_foler").get_to(mustache_folder);
		cur_json_content.at("generated_folder").get_to(generated_folder);
	}
	catch (std::exception& e)
	{
		std::cout << "fail to parse json content with exception " << e.what() << std::endl;
		return 1;
	}
	if (src_file.empty())
	{
		std::cout << "empty src file" << std::endl;
		return 1;
	}
	src_file = file_folder + src_file;
	if (mustache_folder.empty())
	{
		std::cout << "mustache folder  is empty" << std::endl;
		return 1;
	}
	if (mustache_folder[0] != '.')
	{
		std::cout << "mustache folder  " << mustache_folder<<" should begin with ."<<std::endl;
		return 1;
	}
	mustache_folder = file_folder + mustache_folder;

	if (generated_folder.empty())
	{
		std::cout << "generated_folder  is empty" << std::endl;
		return 1;
	}
	if (generated_folder[0] != '.')
	{
		std::cout << "generated_folder  " << generated_folder << " should begin with ." << std::endl;
		return 1;
	}
	generated_folder = file_folder + generated_folder;

	for (auto& one_include_dir : include_dirs)
	{
		if (one_include_dir.empty())
		{
			continue;
		}
		if (one_include_dir[0] == '.')
		{
			one_include_dir = file_folder + one_include_dir;
		}
		
	}
	auto& the_logger = utils::get_logger();
	CXIndex m_index;
	CXTranslationUnit m_translationUnit;
	std::vector<std::string> arguments;

	for (const auto& one_include_dir : include_dirs)
	{
		arguments.push_back("-I" + one_include_dir);
	}
	for (const auto& one_definition : compile_definitions)
	{
		arguments.push_back(one_definition);
	}
	arguments.push_back("-x");
	arguments.push_back("c++");
	arguments.push_back("-std=c++17");
	arguments.push_back("-D__meta_parse__");

	std::vector<const char*> cstr_arguments;

	for (const auto& i : arguments)
	{
		cstr_arguments.push_back(i.c_str());
	}

	bool display_diag = true;
	m_index = clang_createIndex(true, display_diag);

	m_translationUnit = clang_createTranslationUnitFromSourceFile(m_index, src_file.c_str(), static_cast<int>(cstr_arguments.size()), cstr_arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translationUnit);
	the_logger.info("the root cursor is {}", utils::to_string(cursor));
	auto& cur_type_db = language::type_db::instance();
	cur_type_db.create_from_translate_unit(cursor);
	//recursive_print_decl_under_namespace("A");
	cur_type_db.build_class_under_namespace("std");
	cur_type_db.build_class_under_namespace(property_namespace);
	//recursive_print_func_under_namespace("A");
	//recursive_print_class_under_namespace("A");
	json result = language::type_db::instance().to_json();
	ofstream json_out("type_info.json");
	json_out << setw(4) << result << endl;
	std::unordered_map<std::string, std::string> file_content;
	//utils::merge_file_content(file_content, generate_encode_decode());
	generator::merge_file_content(file_content, generate_property(generated_folder, mustache_folder));
	generator::write_content_to_file(file_content);
	clang_disposeTranslationUnit(m_translationUnit);

	return 0;

}
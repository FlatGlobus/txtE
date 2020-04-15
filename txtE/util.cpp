#include "util.h"
#include "chaiscript/extras/string_methods.hpp"

//////////////////////////////////////////////////////////////////////////
ChaiEngine::module_type * ChaiEngine::_modules;
std::unique_ptr<chaiscript::ChaiScript> ChaiEngine::engine;

void ChaiEngine::start()
{
	init_std();
	if (_modules)
	{
		for (auto modul : *_modules)
		{
			modul->register_module();
			ChaiEngine::get_engine()->add(modul->m);
		}
	}
}

void ChaiEngine::stop()
{
	ChaiEngine::engine.release();
}

chaiscript::ChaiScript* ChaiEngine::get_engine()
{
    if (ChaiEngine::engine == nullptr)
        ChaiEngine::engine = std::make_unique<chaiscript::ChaiScript>();
	return ChaiEngine::engine.get();
}

void  ChaiEngine::init_std()
{
    auto stringmethods = chaiscript::extras::string_methods::bootstrap();
    ChaiEngine::get_engine()->add(stringmethods);
    ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("VectorString"));
	ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::string_type<std::wstring>("wstring"));
}


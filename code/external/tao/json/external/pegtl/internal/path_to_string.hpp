// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAO_JSON_PEGTL_INTERNAL_PATH_TO_STRING_HPP
#define TAO_JSON_PEGTL_INTERNAL_PATH_TO_STRING_HPP

#include <filesystem>
#include <string>

#include "../config.hpp"

namespace TAO_JSON_PEGTL_NAMESPACE::internal
{
   [[nodiscard]] inline std::string path_to_string( const std::filesystem::path& path )
   {
#if defined( __cpp_char8_t )
      const auto s = path.u8string();
      return { reinterpret_cast< const char* >( s.data() ), s.size() };
#else
      return path.u8string();
#endif
   }

}  // namespace TAO_JSON_PEGTL_NAMESPACE::internal

#endif

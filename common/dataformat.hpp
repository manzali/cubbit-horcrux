#pragma once

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>

namespace bj = boost::json;

namespace horcrux
{

    namespace dataformat
    {

        enum request_type
        {
            SAVE,
            LOAD,
            UNKNOWN
        };

        request_type get_request_type_from_string(std::string str)
        {
            if (boost::iequals(str, "save"))
            {
                return request_type::SAVE;
            }

            if (boost::iequals(str, "load"))
            {
                return request_type::LOAD;
            }

            return request_type::UNKNOWN;
        }

        std::string const msg_delimiter = "\r\n\r\n";

    }

}

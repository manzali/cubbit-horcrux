#pragma once

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>

namespace bj = boost::json;

namespace horcrux
{

    namespace dataformat
    {

        std::string const field_delimiter = "\r\n";
        std::string const message_delimiter = "\r\n\r\n";

        enum status_code
        {
            SAVE_REQUEST_OK,
            SAVE_REQUEST_FAILED,
            LOAD_REQUEST_OK,
            LOAD_REQUEST_FAILED,
            UNKNOWN_REQUEST
        };

    }

}

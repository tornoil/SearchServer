#include "string_processing.h"

std::vector<std::string_view> SplitIntoWords(std::string_view text) {
    std::vector<std::string_view> result;
    const int64_t pos_end = text.npos;
    while(true) {
        int64_t space = text.find(' ');
        result.push_back(text.substr(0,space));
        text.remove_prefix(space+1);
        if(space == pos_end) {
            break;
        }
    }
    return result;

}

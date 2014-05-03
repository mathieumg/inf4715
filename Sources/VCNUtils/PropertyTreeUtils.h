///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "Types.h"

#include <string>
#include <vector>
#include <functional>

//#include <boost\filesystem\path.hpp>

//forward declarations
namespace boost
{
    namespace property_tree
    {
        template < class Key, class Data, class KeyCompare>
        class basic_ptree;

        typedef basic_ptree<std::string, std::string, std::less<std::string>> ptree;
    }
};

namespace VCNPTree
{
    extern const VCNChar* FILE_ATTRIBUTE;
    extern const VCNChar* ATTRIBUTES_TAG;
    extern const VCNChar* COMMENTS_TAG;

    std::vector<boost::property_tree::ptree> ConvertFilesToTrees(const std::vector<std::string>& paths);
    void PopulatePropertyTreeFromFile(const std::string& path, boost::property_tree::ptree& tree);

    typedef std::function< boost::property_tree::ptree (const boost::property_tree::ptree&) > ReplacementFunc;

    /*
        The function passed in to ReplacementFunc returns a ptree that contains all the children that will replace
        the target.

        Example:

        Original Tree:
            Root
                ToReplace
                Other
                Other

        Returned By ReplacementFunc:

            Replacement
                Item
                Item
                Item

        Result:

            Root
                Item
                Item
                Item
                Other
                Other
    */
    boost::property_tree::ptree TransformChildren(const std::string& targetChildName, boost::property_tree::ptree& tree, ReplacementFunc replacementFunc );

    //Takes the children of the attributes node and places them as children of said node
    //is applied recursively
    void FlattenAttributes(boost::property_tree::ptree& tree);

    typedef std::pair<const std::string, boost::property_tree::ptree> Itr;
};



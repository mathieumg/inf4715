///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "PropertyTreeUtils.h"

#include "Assert.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace VCNPTree
{
	using namespace boost::property_tree;

	const VCNChar* FILE_ATTRIBUTE = "File";
	const VCNChar* ATTRIBUTES_TAG = "<xmlattr>";
	const VCNChar* COMMENTS_TAG = "<xmlcomment>";

	std::vector<boost::property_tree::ptree> ConvertFilesToTrees(const std::vector<std::string>& paths)
	{
		std::vector<ptree> trees;

		std::for_each(paths.begin(), paths.end(), [&trees](const std::string& path)
		{
			boost::property_tree::ptree tree;

#if !defined(FINAL)
			try
			{
#endif
				PopulatePropertyTreeFromFile(path, tree);
				tree.add(FILE_ATTRIBUTE, path);
				trees.push_back(tree);
#if !defined(FINAL)
			}
			catch(const std::exception& exception)
			{
				VCN_ASSERT_MSG( false, "Error loading xml file: %s", exception.what());
			}
#endif
		});

		return trees;
	}

	void PopulatePropertyTreeFromFile(const std::string& path, ptree& tree)
	{
		boost::property_tree::read_xml<ptree>(
			path, tree, boost::property_tree::xml_parser::trim_whitespace);
	}

	void FlattenAttributes(ptree& tree)
	{
		std::for_each(tree.begin(), tree.end(), [&tree]( std::pair<const std::string, ptree>& child )
		{
			if (child.first == ATTRIBUTES_TAG || child.first == COMMENTS_TAG)
				return;

			//recurse through children
			FlattenAttributes(child.second);
		});

		auto optionalAttributes = tree.get_child_optional(ATTRIBUTES_TAG);
		if (!optionalAttributes.is_initialized())
			return;

		ptree& attributes = optionalAttributes.get();

		std::for_each(attributes.begin(), attributes.end(), [&tree]( std::pair<const std::string, ptree>& child )
		{
			tree.add_child(child.first, child.second);
		});
	}

	boost::property_tree::ptree TransformChildren(  const std::string& targetChildName, 
		ptree& tree,
		ReplacementFunc replacementFunc )
	{
		ptree result;

		std::for_each(tree.begin(), tree.end(),
			[&](std::pair<const std::string, ptree>& itr)
		{
			if (itr.first == COMMENTS_TAG)
				return;

			if (itr.first == targetChildName)
			{
				auto replacement = replacementFunc(itr.second);

				ptree& crapResult = result;
				std::for_each( replacement.begin(), replacement.end(),
					[&crapResult](std::pair<const std::string, ptree>& child)
				{
					crapResult.add_child( child.first, child.second );
				});
			}
			else
			{
				result.add_child( itr.first, itr.second );
			}

		});

		return result;
	}
}

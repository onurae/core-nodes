/******************************************************************************************
*                                                                                         *
*    Core Library                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef CORELIBRARY_HPP
#define CORELIBRARY_HPP

#include "CoreNode.hpp"
#include "GainNode.hpp"

class CoreLibrary
{
private:
    std::vector<std::string> libMath = { "Gain", "Abs", "Product", "Test"};

    int iSelectedLeaf = -1;
    int iSelectedBranch = -1;
    std::string selectedLeaf;
    std::string selectedBranch;
    bool leafClicked = false;
    void DrawBranch(const std::string& name, int& id, const std::vector<std::string>& vec);
public:
    CoreLibrary() = default;
    virtual ~CoreLibrary() = default;
    CoreNode* GetNode(const std::string & libName, const std::string & uniqueName);

    void Draw();
    bool IsLeafClicked() const { return leafClicked; }
    void SetLeafClickedFalse() { leafClicked = false; }
    std::string GetSelectedLeaf() const { return selectedLeaf; }
};

#endif /* CORELIBRARY_HPP */
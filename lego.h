#ifndef LEGO_H
#define LEGO_H

#pragma once

#include "Camera.h"
#include <math.h>
#include <QDebug>
#include <tuple>

//Header file which contains all the neccessary information about the Lego blocks

//Enums used for representing every lego size available
enum BlockTypes {
    BS_1x1, BS_1x2, BS_1x3, BS_1x4, BS_1x6, BS_1x8, BS_2x2, BS_2x3, BS_2x4, BS_2x6
};

//Enums used for representing every lego color available
enum Colors {
    Blue, Green, LightBlue, LightGreen, Orange, Purple, Red, White, Yellow, Black
};

//Enums used to represent the orientation of the block, when placed on the plate
enum Orientation {
    Horizontal, Vertical, Square
};

//Static bool variable used to check whether the program is counting how many blocks of a certain type and color are in a mosaic
static bool isCounting = false;

/*
    A struct containing blob parameters based on which the correct brick type is determined
*/
struct BlockParams {
    std::string type;
    BlockTypes enumType;
    int Width;
    int Height;
};

//A struct containing all the information needed to define a lego block color
struct BlockColors {
    std::string color; //used for displaying the name of the color to the user and console
    Colors enumColor; //used for comparing colors of different blocks
    int minH, maxH, minS, maxS, minV, maxV; //HSV values used for detecting the color when a block is placed
    cv::Scalar colorScalar; //RGB representation of a color used to draw bounding boxes in the frame
};

//A struct representing a categorized lego block with a specific color, type, position and orientation
struct LegoBlock {
    BlockTypes enumType;
    std::string type;
    Colors enumColor;
    std::string color;
    Orientation enumOrientation;
    std::string orientation;
    bool isEmpty = true;
    cv::Rect boundingBox;
    cv::Point topLeftCorner;
    int occurrences = 0;

    //Write function required for writing the information about a lego block into a file
    void write(cv::FileStorage& fs) const
    {
        fs << "{" << "type" << type << "enumType" << (int)enumType << "color" << color << "enumColor" << (int)enumColor << "orientation" << orientation
            << "enumOrientation" << (int)enumOrientation << "isEmpty"<< (int)isEmpty << "boundingBox" << boundingBox
            << "topLeftCorner" << topLeftCorner << "occurrences" << occurrences << "}";
    }
    //Read function required for reading the information about a lego block from a file
    void read(const cv::FileNode& node)
    {
        //Take the data from the node with the specific name, cast it to the correct variable type
        //and assign it to the corresponding variable with the same name
        enumType = (BlockTypes)(int)node["enumType"];
        type = (std::string)node["type"];
        enumColor = (Colors)(int)node["enumColor"];
        color = (std::string)node["color"];
        enumOrientation = (Orientation)(int)node["enumOrientation"];
        orientation = (std::string)node["orientation"];
        isEmpty = (bool)(int)node["isEmpty"];
        boundingBox.x = (int)node["boundingBox"][0];
        boundingBox.y = (int)node["boundingBox"][1];
        boundingBox.width = (int)node["boundingBox"][2];
        boundingBox.height = (int)node["boundingBox"][3];
        topLeftCorner.x = (int)node["topLeftCorner"][0];
        topLeftCorner.y = (int)node["topLeftCorner"][1];
        occurrences = node["occurrences"];
    }
};

//These write and read functions must be defined for the serialization in FileStorage to work
//These functions simply call their counterparts in the specific LegoBlock object that has to be saved or loaded
static void write(cv::FileStorage& fs, const std::string&, const LegoBlock& theBlock)
{
    theBlock.write(fs);
}

static void read(const cv::FileNode& node, LegoBlock& theBlock, const LegoBlock& default_value = LegoBlock()) {
    if (node.empty())
        theBlock.isEmpty = true;
    else
        theBlock.read(node);
}

// This function will print the LegoBlock object to the console when using it with cout
static std::ostream& operator<<(std::ostream& out, const LegoBlock& theBlock)
{
    out << "==============================================" << std::endl;
    out << "Block color: " << theBlock.color << std::endl;
    out << "Block type: " << theBlock.type << std::endl;
    out << "Block orientation: " << theBlock.orientation << std::endl;
    out << "Block Top Left Point: " << theBlock.topLeftCorner << std::endl;
    //out << "isEmpty: " << theBlock.isEmpty << endl;
    out << "==============================================" << std::endl;
    return out;
}

//Overloading the == operator so that it can be used when comparing if two blocks are equal
static bool operator==(const LegoBlock& block1, const LegoBlock& block2)
{
    bool areEqual = false;

    //If the program is not counting occurrences of blocks in a mosaic, consider the position of the block when deciding equality
    //If not, consider only type and color
    if(!isCounting){
        int pMargin = 15;
        int pMinX = block1.boundingBox.x - pMargin;
        int pMaxX = block1.boundingBox.x + pMargin;
        int pMinY = block1.boundingBox.y - pMargin;
        int pMaxY = block1.boundingBox.y + pMargin;

        if (block1.enumColor == block2.enumColor) {
            if (block1.enumType == block2.enumType) {
                if (block1.enumOrientation == block2.enumOrientation) {
                    if ((block2.topLeftCorner.x > pMinX && block2.topLeftCorner.x < pMaxX) && (block2.topLeftCorner.y > pMinY && block2.topLeftCorner.y < pMaxY)) {

                        areEqual = true;
                    }
                }
            }
        }
    }
    else{
        if (block1.enumType == block2.enumType) {
            if (block1.enumColor == block2.enumColor) {
                areEqual = true;
            }
        }
    }

    return areEqual;
}

//Overloading the != operator
static bool operator!=(const LegoBlock& block1, const LegoBlock& block2)
{
    bool areEqual = true;
    int pMargin = 15;
    int pMinX = block1.boundingBox.x - pMargin;
    int pMaxX = block1.boundingBox.x + pMargin;
    int pMinY = block1.boundingBox.y - pMargin;
    int pMaxY = block1.boundingBox.y + pMargin;
    cv::Point b2topLeft = block2.topLeftCorner;
    if (block1.enumColor == block2.enumColor) {
        if (block1.enumType == block2.enumType) {
            if (block1.enumOrientation == block2.enumOrientation) {
                if ((b2topLeft.x > pMinX && b2topLeft.x < pMaxX) && (b2topLeft.y > pMinY && b2topLeft.y < pMaxY)) {
                    areEqual = false;
                }
            }
        }
    }

    return areEqual;
}

//Comparator function used for sorting the blocks from top left to bottom right
static bool sortBlockPositions(const LegoBlock& block1, const LegoBlock& block2) {
    return std::tie(block1.boundingBox.y, block1.boundingBox.x) < std::tie(block2.boundingBox.y, block2.boundingBox.x);
}

//Comparator function used for determining if 2 blocks are different, needed for counting block occurrences
static bool isBlockDifferent(const LegoBlock& block1, const LegoBlock& block2) {
    return std::tie(block1.enumType, block1.enumColor) < std::tie(block2.enumType, block2.enumColor);
}

//a set containing LegoBlock objects, a set is a container not allowing duplicates to be inserted, needed for constructing
//the details list when saving or loading a mosaic
extern std::set<LegoBlock, decltype(&isBlockDifferent)> theLegoSet;


#endif // LEGO_H

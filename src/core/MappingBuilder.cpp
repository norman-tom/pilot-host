#include "MappingBuilder.hpp"
#include "topography/URLFactory.hpp"
#include "topography/MapNode.hpp"
#include "topography/MapTree.hpp"
#include <memory>
#include <iostream>

using namespace nectar::core;
using namespace nectar::topography;
using namespace std;

#define nPtr MapNode*
#define addProc(p)  tree.newChild(p, "desc", false, true, false);   \
                    tree.newChild(p, "sp", false, true, true);      \
                    tree.newChild(p, "kp", false, true, true);      \
                    tree.newChild(p, "ki", false, true, true);      \
                    tree.newChild(p, "kd", false, true, true);      \
                    tree.newChild(p, "ts", false, true, true);

Mapping MappingBuilder::build()
{
    Mapping map;
    URLFactory factory(&map);

    MapTree tree;
    nPtr root = tree.getRoot();

        //Module Tree
        nPtr device = tree.newChild(root, "device");
        nPtr hlt = tree.newChild(device, "hlt");
            tree.newChild(hlt, "temp", false, true);
            tree.newChild(hlt, "weight", false, true);
            tree.newChild(hlt, "element", false, true);
            tree.newChild(hlt, "pump", false, true);
        nPtr mash = tree.newChild(device, "mash");
            tree.newChild(mash, "temp", false, true);
            tree.newChild(mash, "weight", false, true);
            tree.newChild(mash, "element");                     //inactive node. Needed so module numbering is correct
            tree.newChild(mash, "pump", false, true);
        nPtr kettle = tree.newChild(device, "kettle");
            tree.newChild(kettle, "temp", false, true);
            tree.newChild(kettle, "weight", false, true);
            tree.newChild(kettle, "element", false, true);
            tree.newChild(kettle, "pump");                      //inactive node.

        //Process Tree
        nPtr process = tree.newChild(root, "process");
            nPtr p257 = tree.newChild(process, "calibrate_hlt", true);
            nPtr p258 = tree.newChild(process, "measure_temp_hlt", true);
                addProc(p258)
            nPtr p259 = tree.newChild(process, "measure_weight_hlt", true);
                addProc(p259)
            nPtr p260 = tree.newChild(process, "manual_element_hlt", true);
            nPtr p261 = tree.newChild(process, "manual_pump_hlt", true);
            nPtr p262 = tree.newChild(process, "control_temp_hlt", true);
                addProc(p262)
            nPtr p263 = tree.newChild(process, "control_weight_hlt", true);
                addProc(p263)

            nPtr p264 = tree.newChild(process, "calibrate_mash", true);
            nPtr p265 = tree.newChild(process, "measure_temp_mash", true);
                addProc(p265)
            nPtr p266 = tree.newChild(process, "measure_weight_mash", true);
                addProc(p266)
            nPtr p267 = tree.newChild(process, "manual_pump_mash", true);
            nPtr p268 = tree.newChild(process, "control_weight_mash", true);
                addProc(p268);

            nPtr p269 = tree.newChild(process, "calibrate_kettle", true);
            nPtr p270 = tree.newChild(process, "measure_temp_kettle", true);
                addProc(p270)
            nPtr p271 = tree.newChild(process, "measure_weight_kettle", true);
                addProc(p271)
            nPtr p272 = tree.newChild(process, "boil_kettle", true);
                addProc(p272)
            nPtr p273 = tree.newChild(process, "manual_element_kettle", true);
            nPtr p274 = tree.newChild(process, "control_weight_kettle", true);
                addProc(p274);

        factory.produce(tree);

    return map;
}
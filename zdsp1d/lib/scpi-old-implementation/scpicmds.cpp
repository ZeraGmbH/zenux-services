// kommando knoten  für zdsp1 hardware server
// es werden alle kommando knoten für zhserver nodes,
// scpi nodes, commands und , queries für den 
// bearbeitenden kommando interpreter definiert

#include "scpi-zdsp.h" // für scpi knoten
#include "scpicmds.h" // scpi erweiterung

cNodeSCPI* System;

// cNodeScpi (QString,tNodeSpec,cNode*,cNode*,SCPICmdType,SCPICmdType); 
// konstruktor, sNodeName, nNodedef, pNextNode, pNewLevelNode, Cmd, Query				
// konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query

cNode* InitCmdTree()
{
    // implementiertes system modell
    System=new cNodeSCPI("SYSTEM",isNode,NULL,NULL,nixCmd,nixCmd);
    return (System);  
}

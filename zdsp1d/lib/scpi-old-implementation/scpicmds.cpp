// kommando knoten  für zdsp1 hardware server
// es werden alle kommando knoten für zhserver nodes,
// scpi nodes, commands und , queries für den 
// bearbeitenden kommando interpreter definiert

#include "scpi-zdsp.h" // für scpi knoten
#include "scpicmds.h" // scpi erweiterung

// die vollständige scpi kommando liste

cNodeSCPI* System;
	     cNodeSCPI* SystemDsp;
		          cNodeSCPI* SystemDspTrigger;
			              cNodeSCPI* SystemDspTriggerIntList;
			          	                   cNodeSCPI* SystemDspTriggerHKSK;

// cNodeScpi (QString,tNodeSpec,cNode*,cNode*,SCPICmdType,SCPICmdType); 
// konstruktor, sNodeName, nNodedef, pNextNode, pNewLevelNode, Cmd, Query				
// konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query

cNode* InitCmdTree()
{
    // implementiertes system modell
    SystemDspTriggerHKSK=new cNodeSCPI("HKSK",isCommand,NULL,NULL,TriggerIntListHKSK,nixCmd);
    SystemDspTriggerIntList=new cNodeSCPI("INTLIST",isNode,NULL,SystemDspTriggerHKSK,nixCmd,nixCmd);
    SystemDspTrigger=new cNodeSCPI("TRIGGER",isNode,NULL,SystemDspTriggerIntList,nixCmd,nixCmd);
    SystemDsp=new cNodeSCPI("DSP",isNode,NULL,SystemDspTrigger,nixCmd,nixCmd);
    System=new cNodeSCPI("SYSTEM",isNode,NULL,SystemDsp,nixCmd,nixCmd);
    return (System);  
}

// kommando knoten  für zdsp1 hardware server
// es werden alle kommando knoten für zhserver nodes,
// scpi nodes, commands und , queries für den 
// bearbeitenden kommando interpreter definiert

#include "scpi-zdsp.h" // für scpi knoten
#include "scpicmds.h" // scpi erweiterung

// die vollständige scpi kommando liste

cNodeSCPI* System;
	     cNodeSCPI* SystemDsp;
                            cNodeSCPI* SystemDspTest;
		          cNodeSCPI* SystemDspTrigger;
			              cNodeSCPI* SystemDspTriggerIntList;
				                   cNodeSCPI* SystemDspTriggerALL;
			          	                   cNodeSCPI* SystemDspTriggerHKSK;
		          cNodeSCPI* SystemDspCommand;
			              cNodeSCPI* SystemDspCommandStat;

// cNodeScpi (QString,tNodeSpec,cNode*,cNode*,SCPICmdType,SCPICmdType); 
// konstruktor, sNodeName, nNodedef, pNextNode, pNewLevelNode, Cmd, Query				
// konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query

cNode* InitCmdTree()
{
    // implementiertes system modell
    SystemDspCommandStat=new cNodeSCPI("STAT",isQuery | isCommand,NULL,NULL,SetDspCommandStat,GetDspCommandStat);      
    SystemDspCommand=new cNodeSCPI("COMMAND",isNode,NULL,SystemDspCommandStat,nixCmd,nixCmd);

    SystemDspTriggerHKSK=new cNodeSCPI("HKSK",isCommand,NULL,NULL,TriggerIntListHKSK,nixCmd);	      
    SystemDspTriggerALL=new cNodeSCPI("ALL",isCommand,SystemDspTriggerHKSK,NULL,TriggerIntListALL,nixCmd);
    SystemDspTriggerIntList=new cNodeSCPI("INTLIST",isNode,NULL,SystemDspTriggerALL,nixCmd,nixCmd);
    SystemDspTrigger=new cNodeSCPI("TRIGGER",isNode,SystemDspCommand,SystemDspTriggerIntList,nixCmd,nixCmd);
    SystemDspTest=new cNodeSCPI("TEST",isCommand,SystemDspTrigger,NULL,TestDsp,nixCmd);
    SystemDsp=new cNodeSCPI("DSP",isNode,NULL,SystemDspTest,nixCmd,nixCmd);
    System=new cNodeSCPI("SYSTEM",isNode,NULL,SystemDsp,nixCmd,nixCmd);
    return (System);  
}

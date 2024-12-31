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
		          cNodeSCPI* SystemDspSampling;	
		          cNodeSCPI* SystemDspTrigger;
			              cNodeSCPI* SystemDspTriggerIntList;
				                   cNodeSCPI* SystemDspTriggerALL;
			          	                   cNodeSCPI* SystemDspTriggerHKSK;
		          cNodeSCPI* SystemDspCommand;
			              cNodeSCPI* SystemDspCommandStat;

cNodeSCPI* Status;
                   cNodeSCPI* StatusDevice;
	     cNodeSCPI* StatusDsp;	   
		          cNodeSCPI* StatusDspLoad;	   
	                                           cNodeSCPI* StatusDspLoadActual;
		                             cNodeSCPI* StatusDspLoadMaximum;
			                                  cNodeSCPI* StatusDspLoadMaximumReset;
	     
cNodeSCPI* MeasureNode;

// cNodeScpi (QString,tNodeSpec,cNode*,cNode*,SCPICmdType,SCPICmdType); 
// konstruktor, sNodeName, nNodedef, pNextNode, pNewLevelNode, Cmd, Query				
// konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query

cNode* InitCmdTree()
{
    // implementiertes measure modell
    MeasureNode=new cNodeSCPI("MEASURE",isNode | isCommand,NULL,NULL,Measure,nixCmd);

    // implementiertes status modell
    StatusDspLoadMaximumReset=new cNodeSCPI("RESET",isCommand,NULL,NULL,ResetDeviceLoadMax,nixCmd);
    StatusDspLoadMaximum=new cNodeSCPI("MAXIMUM",isNode | isQuery,NULL,StatusDspLoadMaximumReset,nixCmd,GetDeviceLoadMax);	
    StatusDspLoadActual=new cNodeSCPI("ACTUAL",isQuery,StatusDspLoadMaximum,NULL,nixCmd,GetDeviceLoadAct);
    StatusDspLoad=new cNodeSCPI("LOAD",isNode,NULL,StatusDspLoadActual,nixCmd,nixCmd);
    StatusDsp=new cNodeSCPI("DSP",isNode | isQuery,NULL,StatusDspLoad,nixCmd,GetDspStatus);
    StatusDevice=new cNodeSCPI("DEVICE",isQuery,StatusDsp,NULL,nixCmd,GetDeviceStatus);
    Status=new cNodeSCPI("STATUS",isNode,MeasureNode,StatusDevice,nixCmd,nixCmd);
    
    // implementiertes system modell
    SystemDspCommandStat=new cNodeSCPI("STAT",isQuery | isCommand,NULL,NULL,SetDspCommandStat,GetDspCommandStat);      
    SystemDspCommand=new cNodeSCPI("COMMAND",isNode,NULL,SystemDspCommandStat,nixCmd,nixCmd);

    SystemDspTriggerHKSK=new cNodeSCPI("HKSK",isCommand,NULL,NULL,TriggerIntListHKSK,nixCmd);	      
    SystemDspTriggerALL=new cNodeSCPI("ALL",isCommand,SystemDspTriggerHKSK,NULL,TriggerIntListALL,nixCmd);
    SystemDspTriggerIntList=new cNodeSCPI("INTLIST",isNode,NULL,SystemDspTriggerALL,nixCmd,nixCmd);
    SystemDspTrigger=new cNodeSCPI("TRIGGER",isNode,SystemDspCommand,SystemDspTriggerIntList,nixCmd,nixCmd);
    SystemDspSampling=new cNodeSCPI("SAMPLING",isQuery | isCommand,SystemDspTrigger,NULL,SetSamplingSystem,GetSamplingSystem);
    SystemDspTest=new cNodeSCPI("TEST",isCommand,SystemDspSampling,NULL,TestDsp,nixCmd);
    SystemDsp=new cNodeSCPI("DSP",isNode,NULL,SystemDspTest,nixCmd,nixCmd);
    System=new cNodeSCPI("SYSTEM",isNode,Status,SystemDsp,nixCmd,nixCmd);
    return (System);  
}

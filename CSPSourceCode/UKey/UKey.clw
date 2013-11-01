; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CUKeyApp
LastClass=CChangePin
NewFileInclude2=#include "UKey.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Resource1=IDD_DIALOG1
Class2=CCheckPin
LastTemplate=CDialog
Class3=CChangePin
Resource2=IDD_DIALOG2

[CLS:CUKeyApp]
Type=0
HeaderFile=UKey.h
ImplementationFile=UKey.cpp
Filter=N
LastObject=CUKeyApp

[DLG:IDD_DIALOG1]
Type=1
Class=CCheckPin
ControlCount=7
Control1=IDOK,button,1342242827
Control2=IDCANCEL,button,1342242827
Control3=IDC_STATICCAPTION,static,1342308352
Control4=IDC_EDIT1,edit,1350639776
Control5=IDC_BUTTON1,button,1342242827
Control6=IDC_STATIC,static,1342177795
Control7=IDC_STATIC,button,1342177287

[DLG:IDD_DIALOG2]
Type=1
Class=CChangePin
ControlCount=12
Control1=IDOK,button,1342242827
Control2=IDCANCEL,button,1342242827
Control3=IDC_STATICOLDPIN,static,1342308352
Control4=IDC_STATICNEWPIN1,static,1342308352
Control5=IDC_STATICNEWPIN2,static,1342308352
Control6=IDC_EDIT1,edit,1350639776
Control7=IDC_EDIT2,edit,1350639776
Control8=IDC_EDIT3,edit,1350639776
Control9=IDC_STATIC,button,1342210055
Control10=IDC_STATIC,static,1342177287
Control11=IDC_STATIC,static,1342177283
Control12=IDC_STATIC,static,1342308352

[CLS:CCheckPin]
Type=0
HeaderFile=CheckPin.h
ImplementationFile=CheckPin.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CCheckPin

[CLS:CChangePin]
Type=0
HeaderFile=ChangePin.h
ImplementationFile=ChangePin.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CChangePin


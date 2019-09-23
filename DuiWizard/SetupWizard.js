// Windows Template Library - DuiVision version 9.0
// Copyright (C) Microsoft Corporation, DuiVision Team. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

// DuiVision App Wizard universal setup program for Visual Studio

main();

function main()
{
    // Decode command line arguments
    var bDebug = false;
    var bElevated = false;
    var Args = WScript.Arguments;
    for(var i = 0; i < Args.length; i++)
    {
        if(Args(i) == "/debug")
            bDebug = true;
        else if(Args(i) == "/elevated")
            bElevated = true;
    }

    // See if UAC is enabled
    var Shell = WScript.CreateObject("Shell.Application");
    if(!bElevated && Shell.IsRestricted("System", "EnableLUA"))
    {
        // Check that the script is being run interactively.
        if(!WScript.Interactive)
        {
            WScript.Echo("ERROR: Elevation required.");
            return;
        }

        // Now relaunch the script, using the "RunAs" verb to elevate
        var strParams = "\"" + WScript.ScriptFullName + "\"";
        if (bDebug)
            strParams += " /debug";
        strParams += " /elevated";
        Shell.ShellExecute(WScript.FullName, strParams, null, "RunAs");
        return;
    }

    // Create shell object
    var WSShell = WScript.CreateObject("WScript.Shell");
    // Create file system object
    var FileSys = WScript.CreateObject("Scripting.FileSystemObject");

    // Get the folder containing the script file
    var strValue = FileSys.GetParentFolderName(WScript.ScriptFullName);
    if(strValue == null || strValue == "")
        strValue = ".";

    var strSourceFolder = FileSys.BuildPath(strValue, "DuiVisionWizard");    // Files
    if(bDebug)
        WScript.Echo("Source: " + strSourceFolder);

    if(!FileSys.FolderExists(strSourceFolder))
    {
        WScript.Echo("ERROR: Cannot find Wizard folder (should be: " + strSourceFolder + ")");
        return;
    }

    MessageBox(WSShell, "Setup will search for installed versions of Visual Studio,\nand ask to add the DuiVision App Wizard for each of them.");

    var strRegKey_32 = "HKLM\\Software\\";
    var strRegKey_64 = "HKLM\\Software\\Wow6432Node\\";

    var nVersions = 14;

    var astrRegKeyVer = new Array();
    astrRegKeyVer[0] = "Microsoft\\VisualStudio\\7.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[1] = "Microsoft\\VisualStudio\\7.1\\Setup\\VC\\ProductDir";
    astrRegKeyVer[2] = "Microsoft\\VisualStudio\\8.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[3] = "Microsoft\\VisualStudio\\9.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[4] = "Microsoft\\VisualStudio\\10.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[5] = "Microsoft\\VisualStudio\\11.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[6] = "Microsoft\\VisualStudio\\12.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[7] = "Microsoft\\VisualStudio\\14.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[8] = "Microsoft\\VCExpress\\8.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[9] = "Microsoft\\VCExpress\\9.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[10] = "Microsoft\\VCExpress\\10.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[11] = "Microsoft\\VCExpress\\11.0\\Setup\\VC\\ProductDir";
    astrRegKeyVer[12] = "Microsoft\\VCExpress\\12.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[13] = "Microsoft\\VCExpress\\14.0\\Setup\\VC\\ProductDir";
	astrRegKeyVer[14] = "Microsoft\\VCExpress\\16.0\\Setup\\VC\\ProductDir";

    var astrFolder = new Array();
    astrFolder[0] = "vcprojects";
    astrFolder[1] = "vcprojects";
    astrFolder[2] = "vcprojects";
    astrFolder[3] = "vcprojects";
    astrFolder[4] = "vcprojects";
    astrFolder[5] = "vcprojects";
    astrFolder[6] = "vcprojects";
	astrFolder[7] = "vcprojects";
    astrFolder[8] = "Express\\vcprojects";
    astrFolder[9] = "Express\\vcprojects";
    astrFolder[10] = "Express\\vcprojects";
    astrFolder[11] = "vcprojects_WDExpress";
    astrFolder[12] = "vcprojects_WDExpress";
	astrFolder[13] = "vcprojects_WDExpress";
	astrFolder[14] = "vcprojects_WDExpress";

    var astrVersions = new Array();
    astrVersions[0] = "Visual Studio 2002 (7.0)";
    astrVersions[1] = "Visual Studio 2003 (7.1)";
    astrVersions[2] = "Visual Studio 2005 (8.0)";
    astrVersions[3] = "Visual Studio 2008 (9.0)";
    astrVersions[4] = "Visual Studio 2010 (10.0)";
    astrVersions[5] = "Visual Studio 2012 (11.0)";
    astrVersions[6] = "Visual Studio 2013 (12.0)";
	astrVersions[7] = "Visual Studio 2015 (14.0)";
    astrVersions[8] = "Visual Studio 2005 Express (8.0)";
    astrVersions[9] = "Visual Studio 2008 Express (9.0)";
    astrVersions[10] = "Visual Studio 2010 Express (10.0)";
    astrVersions[11] = "Visual Studio 2012 Express (11.0)";
    astrVersions[12] = "Visual Studio 2013 Express (12.0)";
	astrVersions[13] = "Visual Studio 2015 Express (14.0)";
	astrVersions[14] = "Visual Studio 2019 Express (16.0)";

    var abExpress = new Array();
    abExpress[0] = false;
    abExpress[1] = false;
    abExpress[2] = false;
    abExpress[3] = false;
    abExpress[4] = false;
    abExpress[5] = false;
    abExpress[6] = false;
	abExpress[7] = false;
    abExpress[8] = true;
    abExpress[9] = true;
    abExpress[10] = true;
    abExpress[11] = true;
	abExpress[12] = true;
	abExpress[13] = true;
	abExpress[14] = true;

    var astrWizVer = new Array();
    astrWizVer[0] = "7.0";
    astrWizVer[1] = "7.1";
    astrWizVer[2] = "8.0";
    astrWizVer[3] = "9.0";
    astrWizVer[4] = "10.0";
    astrWizVer[5] = "11.0";
    astrWizVer[6] = "12.0";
	astrWizVer[7] = "14.0";
    astrWizVer[8] = "8.0";
    astrWizVer[9] = "9.0";
    astrWizVer[10] = "10.0";
    astrWizVer[11] = "11.0";
    astrWizVer[12] = "12.0";
	astrWizVer[13] = "14.0";
	astrWizVer[14] = "16.0";

    var nSpecial = 4;

    var nWizSpecial = 0;

    var bFound = false;
    for(var i = 0; i < nVersions; i++)
    {
        if(bDebug)
            WScript.Echo("Looking for: " + astrVersions[i]);

        try
        {
            var strVCKey = strRegKey_32 + astrRegKeyVer[i];
            strValue = WSShell.RegRead(strVCKey);
        }
        catch(e)
        {
            try
            {
                var strVCKey_x64 = strRegKey_64 + astrRegKeyVer[i];
                strValue = WSShell.RegRead(strVCKey_x64);
            }
            catch(e)
            {
                continue;
            }
        }

        var strDestFolder = FileSys.BuildPath(strValue, astrFolder[i]);
        if(bDebug)
            WScript.Echo("Destination: " + strDestFolder);
        if(!FileSys.FolderExists(strDestFolder))
            continue;

        if(i == nSpecial)   // special case for VS2010
        {
            var strCheckFile = FileSys.BuildPath(strDestFolder, "vc.vsdir");
            if(!FileSys.FileExists(strCheckFile))
                continue;
        }

        bFound = true;
        var strMsg = "Found: " + astrVersions[i] + "\n\nInstall DuiVision App Wizard?";
        var bRet = MessageBox(WSShell, strMsg, true);
        if(bRet)
        {
            var bWizSpecial = (i == nWizSpecial);   // special case for VS2002
            SetupWizard(WSShell, FileSys, strSourceFolder, strDestFolder, astrWizVer[i], bWizSpecial, abExpress[i], bDebug);
        }
    }

    if(bFound)
        MessageBox(WSShell, "Done!");
    else
        MessageBox(WSShell, "Setup could not find Visual Studio installed");
}

function MessageBox(WSShell, strText, bYesNo)
{
    var nType = bYesNo ? (4 + 32) : 0;   // 4 = Yes/No buttons, 32 = Questionmark icon, 0 = OK button
    var nRetBtn = WSShell.Popup(strText, 0, "DuiVision App Wizard Setup", nType);
    return (nRetBtn == 6);   // 6 = Yes;
}

function SetupWizard(WSShell, FileSys, strSourceFolder, strDestFolder, strWizVer, bWizSpecial, bExpress, bDebug)
{
    // Copy files
    try
    {
        var strSrc = FileSys.BuildPath(strSourceFolder, "DuiVisionWizard.ico");
        var strDest = FileSys.BuildPath(strDestFolder, "DuiVisionWizard.ico");
        FileSys.CopyFile(strSrc, strDest);

        strSrc = FileSys.BuildPath(strSourceFolder, "DuiVisionWizard.vsdir");
        strDest = FileSys.BuildPath(strDestFolder, "DuiVisionWizard.vsdir");
        FileSys.CopyFile(strSrc, strDest);
    }
    catch(e)
    {
        var strError = "no info";
        if(e.description.length != 0)
            strError = e.description;
        WScript.Echo("ERROR: Cannot copy file (" + strError + ")");
        return;
    }

    // Read and write DuiVisionWizard.vsz, add engine version and replace path when found
    try
    {
        var strSrc = FileSys.BuildPath(strSourceFolder, "DuiVisionWizard.vsz");
        var strDest = FileSys.BuildPath(strDestFolder, "DuiVisionWizard.vsz");

        var ForReading = 1;
        var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
        if(fileSrc == null)
        {
            WScript.Echo("ERROR: Cannot open source file " + strSrc);
            return;
        }

        var ForWriting = 2;
        var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
        if(fileDest == null)
        {
            WScript.Echo("ERROR: Cannot open destination file" + strDest);
            return;
        }

        while(!fileSrc.AtEndOfStream)
        {
            var strLine = fileSrc.ReadLine();
            if(!bWizSpecial && (strLine.indexOf("Wizard=VsWizard.VsWizardEngine") != -1))
                strLine += "." + strWizVer;
            else if(strLine.indexOf("WIZARD_VERSION") != -1)
                strLine = "Param=\"WIZARD_VERSION = " + strWizVer + "\"";
            else if(strLine.indexOf("ABSOLUTE_PATH") != -1)
                strLine = "Param=\"ABSOLUTE_PATH = " + strSourceFolder + "\"";
            fileDest.WriteLine(strLine);
        }

        if(bExpress)
            fileDest.WriteLine("Param=\"VC_EXPRESS = 1\"");

        fileSrc.Close();
        fileDest.Close();
    }
    catch(e)
    {
        var strError = "no info";
        if(e.description.length != 0)
            strError = e.description;
        WScript.Echo("ERROR: Cannot read and write DuiVisionWizard.vsz (" + strError + ")");
        return;
    }

    // Create DuiVision folder
    var strDestDUIFolder = "";
    try
    {
        strDestDUIFolder = FileSys.BuildPath(strDestFolder, "DuiVision");
        if(!FileSys.FolderExists(strDestDUIFolder))
            FileSys.CreateFolder(strDestDUIFolder);
        if(bDebug)
            WScript.Echo("DuiVision Folder: " + strDestDUIFolder);
    }
    catch(e)
    {
        var strError = "no info";
        if(e.description.length != 0)
            strError = e.description;
        WScript.Echo("ERROR: Cannot create DuiVision folder (" + strError + ")");
        return;
    }

    // Read and write additional DuiVisionWizard.vsdir, add path to the wizard location
    try
    {
        var strSrc = FileSys.BuildPath(strSourceFolder, "DuiVisionWizard.vsdir");
        var strDest = FileSys.BuildPath(strDestDUIFolder, "DuiVisionWizard.vsdir");

        var ForReading = 1;
        var fileSrc = FileSys.OpenTextFile(strSrc, ForReading);
        if(fileSrc == null)
        {
            WScript.Echo("ERROR: Cannot open source file " + strSrc);
            return;
        }

        var ForWriting = 2;
        var fileDest = FileSys.OpenTextFile(strDest, ForWriting, true);
        if(fileDest == null)
        {
            WScript.Echo("ERROR: Cannot open destination file" + strDest);
            return;
        }

        while(!fileSrc.AtEndOfStream)
        {
            var strLine = fileSrc.ReadLine();
            if(strLine.indexOf("DuiVisionWizard.vsz|") != -1)
                strLine = "..\\" + strLine;
            fileDest.WriteLine(strLine);
        }

        fileSrc.Close();
        fileDest.Close();
    }
    catch(e)
    {
        var strError = "no info";
        if(e.description.length != 0)
            strError = e.description;
        WScript.Echo("ERROR: Cannot read and write DuiVision\\DuiVisionWizard.vsdir (" + strError + ")");
        return;
    }

    WScript.Echo("App Wizard successfully installed!");
}

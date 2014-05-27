When running Visual Basic code from LabVIEW, 


To create a new Visual Basic DLL:

 1. Create a new project in Visual Studio
    (select "Class Library" under Visual Basic)

 2. Once the project has been created, go to: File -> Save All
    and save the project in the desired location.

 3. Rename the class and the file containing the class.

 4. Go to Build -> Configuration Manager and set the "Active Solution Configuration"
    to "Release" This will improve the code's performance.

 5. Add the code to create whatever classes you need.

 6. To compile the code, go to Build -> Build <projectName>.
    (If you want to rebuild the Visual Studio project when it is already
     being used by a LabVIEW VI, close the VI or LabVIEW project that uses
     the .dll, and then rebuild. Any changes you make will automatically
     appear in the LabVIEW code.)

 7. To access the code in LabVIEW, go to the block diagram. Then, navigate
    to the Connectivity -> .NET pallete, and choose the Constructor VI.

 8. Click "Browse", then find the root directory of the Visual Basic project.
    Open the folder with your project's name, and navigate to "bin" -> "Release",
    and select the .dll file in this folder.

 9. Select the class you want to construct, and click OK.

 10. To access a data member of the class, go back to the .NET pallete,
     and select the "Property Node" VI.

 11. To call a method of the class, use the Invoke Node in the .NEW pallete.

 12. To destroy the class when you are finished using it, use the close reference VI. 




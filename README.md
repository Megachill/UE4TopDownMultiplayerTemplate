### C++ TopDown Multiplayer Template - 4.14.1

This Template is made for those who want to know how to create a multiplayer version of EPICs TopDown Template. This is C++ and not Blueprint. If you would like to see a blueprinted variant please go to the following URL:

https://answers.unrealengine.com/questions/34074/does-ue4-have-client-side-prediction-built-in.html

This C++ version is using a very similar approach to what you can see in the blueprint solution. 

#### Important Note:

Take a look at the Tick function inside TDMPCharacterProxy, if you set the ScalarRegister higher than 0.01f the proxy pawn and everything attached to it will have epileptic seizures and you'll be wondering what in the name of "insert favorite deity here" is going on here?

```c++
Transform.LerpTranslationScale3D(CharTransform, MyTransform, ScalarRegister(0.01f));
```

Any questions please go over to the thread on the EPIC forums.

https://forums.unrealengine.com/showthread.php?131269-TEMPLATE-Multiplayer-TopDown-Template-C

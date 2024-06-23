Scriptname OM_MarkerScript extends ObjectReference  

EffectShader Property OM_Valid  Auto  

EffectShader Property OM_Error  Auto  


Function SetValid()
	OM_Error.stop(self)
	OM_Valid.play(self)
EndFunction

Function SetError()
	OM_Valid.stop(self)
	OM_Error.play(self)
EndFunction
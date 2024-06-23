Scriptname OM_MarkerScript extends ObjectReference  

EffectShader Property OM_Valid  Auto  

EffectShader Property OM_Error  Auto  


EffectShader Last = None

Event OnLoad()
	if(Last)
		Last.Play(self)
	endif
endEvent

Function SetValid()
	OM_Error.stop(self)
	OM_Valid.play(self)
	Last = OM_Valid
EndFunction

Function SetError()
	OM_Valid.stop(self)
	OM_Error.play(self)
	Last = OM_Error
EndFunction


Function Destroy()
	self.Delete()
EndFunction
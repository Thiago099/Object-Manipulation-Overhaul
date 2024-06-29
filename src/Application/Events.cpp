#include "Application/Events.h"



void Events::InitLoadGame::thunk() { 
    logger::info("load init"); 
    originalFunction();
}

void Events::FinishLoadGame::thunk() { 
    logger::info("load finish"); 
    originalFunction();
}

void Events::Install() {
    auto builder = new HookBuilder();
    builder->AddCall<InitLoadGame, 5, 14>(36643, 0x22, 37651, 0x22);
    builder->AddCall<FinishLoadGame, 5, 14>(36644, 0x39, 37652, 0x36);
    builder->Install();
    delete builder;
}

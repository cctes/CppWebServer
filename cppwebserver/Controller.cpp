#include "Controller.h"
#include "demo.h"


//调用RegisterSProcessor 将处理器放进进控制器
//处理器必须继承processor
Controller::Controller() {
	BaseController::BaseController();

	RegisterSProcessor(new myProcessor());

}
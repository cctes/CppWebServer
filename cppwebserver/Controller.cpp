#include "Controller.h"
#include "demo.h"


//����RegisterSProcessor ���������Ž���������
//����������̳�processor
Controller::Controller() {
	BaseController::BaseController();

	RegisterSProcessor(new myProcessor());

}
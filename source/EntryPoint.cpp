#pragma once
#include"Application.h"

extern ntn::Application* CreateApplication();

int main()
{
	std::unique_ptr<ntn::Application> app(ntn::CreateApplication());
	if (app)
	{
		app->run();
	}


	return 0;
}
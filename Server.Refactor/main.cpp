#include "main.h"
#include "app.h"

App * gApp = nullptr;

int main(int argc, char *argv[])
{
    return App::exec(argc,argv);
}

#include <iostream>
#include "global.h"
#include "tfileio.h"

scenario_data_type scenario;

int main(int argc, char *argv[])
{

    if (argc != 3){
    printf("Usage : %s scenname.exs password\nPlease enter the name of the scenario with extension, followed by the password to be set (0 for none)",argv[0]);
    return EXIT_SUCCESS;
    }

    if(load_scenario(argv)== FALSE){
        printf("Error loading the scenario.\n");
        return  EXIT_SUCCESS;
    }
    if(save_scenario(argv)== FALSE){
        printf("Error writing the scenario.\n");
        return  EXIT_SUCCESS;
    }    
    if(strcmp("0",argv[2])== 0)
    printf("The %s scenario's password has been removed.\n",argv[1]);
    else
    printf("The %s scenario's password is now %s.\n",argv[1],argv[2]);
    return EXIT_SUCCESS;
}

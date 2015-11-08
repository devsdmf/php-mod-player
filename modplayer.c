/**
 * PHP CLI AUDIO STREAMMER
 *
 * This is a extension to provide a stream of module audio files under
 * the CLI SAPI. This library is licensed under the MIT license.
 * 
 * Developed by: devSDMF <devsdmf@gmail.com>
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "php.h"
#include "php_ini.h"
#include "php_modplayer.h"
#include "mikmod.h"

ZEND_DECLARE_MODULE_GLOBALS(modplayer)

static zend_function_entry modplayer_functions[] = {
    PHP_FE(play_module_file, NULL)
    PHP_FE(mod_player_getpid, NULL)
    PHP_FE(stop_module_file, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry modplayer_module_entry = {
    #if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
    #endif
    PHP_MODPLAYER_EXTNAME,
    modplayer_functions,
    PHP_MINIT(modplayer),
    PHP_MSHUTDOWN(modplayer),
    NULL,
    NULL,
    NULL,
    #if ZEND_MODULE_API_NO >= 20010901
    PHP_MODPLAYER_VERSION,
    #endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MODPLAYER
ZEND_GET_MODULE(modplayer)
#endif

PHP_INI_BEGIN()
PHP_INI_ENTRY("modplayer.maxchan","64",PHP_INI_ALL,NULL)
PHP_INI_ENTRY("modplayer.curious","0",PHP_INI_ALL,NULL)
PHP_INI_END()

static void php_modplayer_init_globals(zend_modplayer_globals *modplayer_globals)
{
    modplayer_globals->pid = 0;
}

PHP_MINIT_FUNCTION(modplayer)
{
    ZEND_INIT_MODULE_GLOBALS(modplayer, php_modplayer_init_globals, NULL);
    REGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(modplayer)
{
    if (MODPLAYER_G(pid) > 0) {
        kill(MODPLAYER_G(pid), SIGKILL);
    }
    
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_FUNCTION(play_module_file)
{
    FILE *fptr;
    char *filename;
    int filename_length, s_pid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_length) == FAILURE) {
        RETURN_NULL();
    }

    if (MODPLAYER_G(pid) > 0) {
        zend_error(E_ERROR, "You've already started the module player");
        RETURN_NULL();
    }

    fptr = fopen(filename, "rb");
    if (fptr == NULL) {
        perror("Error");
        zend_error(E_ERROR, "An error occurred at try to open the module audio file specified");
        RETURN_NULL();
    }

    s_pid = stream_audio(fptr, INI_INT("modplayer.maxchan"), INI_INT("modplayer.curious"));

    if (s_pid > 0) {
        MODPLAYER_G(pid) = s_pid;
    }

    RETURN_LONG(s_pid);
}

PHP_FUNCTION(mod_player_getpid)
{
    RETURN_LONG(MODPLAYER_G(pid));
}

PHP_FUNCTION(stop_module_file)
{
    if (MODPLAYER_G(pid) > 0) {
        kill(MODPLAYER_G(pid), SIGKILL);
        MODPLAYER_G(pid) = 0;
    }

    RETURN_NULL();
}

int stream_audio(FILE *fptr, int maxchan, int curious)
{
    pid_t m_pid;
    signed int s_pid;

    m_pid = fork();
    if (m_pid == 0) {
        MODULE *module;

        MikMod_InitThreads();
        MikMod_RegisterAllDrivers();
        MikMod_RegisterAllLoaders();

        md_mode |= DMODE_SOFT_MUSIC | DMODE_NOISEREDUCTION | DMODE_INTERP;
        if (MikMod_Init("")) {
            zend_error(E_ERROR, "Could not initialize the MikMod library");
            return -1;
        }

        module = Player_LoadFP(fptr, maxchan, curious);
        if (module) {
            module->wrap = 1;
            module->loop = 0;

            Player_Start(module);

            s_pid = getpid();
            MODPLAYER_G(pid) = s_pid;

            while (Player_Active()) {
                usleep(10000);
                MikMod_Update();
            }
        } else {
            zend_error(E_ERROR, "Could not load module");
            return -1;
        }

        fclose(fptr);
        MikMod_Exit();
    } else if (m_pid < 0) {
        zend_error(E_ERROR, "Failed to fork CLI audio stream process");
        return -1;
    }

    return m_pid;
}
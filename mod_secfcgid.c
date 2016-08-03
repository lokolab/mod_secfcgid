/**
 * Module for Apache HTTP Server.
 *
 * Copyright (c) Krystian Pietruszka <kpietru@lokolab.net>
 * License MIT
 */



#include "http_core.h"
#include "http_log.h"



// Configuration structure.
typedef struct
{
    int bad_wrap;
    int bad_directive;
} secfcgid_config;



// Prototypes
static void register_hooks(apr_pool_t *pool);
static int  secfcgid_handler(request_rec *r);
const char  *set_check_path_take123(cmd_parms *cmd, void *config, const char *arg1, const char *arg2, const char *arg3);
const char  *set_bad_directive_take1(cmd_parms *cmd, void *config, const char *arg);
const char  *set_bad_directive_flag(cmd_parms *cmd, void *config, int arg);
void        *create_dir_conf(apr_pool_t *pool, char *context);



// Configuration directives.
static const command_rec directives[] =
{
    // FcgidWrapper
    AP_INIT_TAKE123("FcgidWrapper", set_check_path_take123, NULL,
        /* RSRC_CONF | ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_TAKE123("FcgiWrapper", set_check_path_take123, NULL,
        /* RSRC_CONF | ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAccessChecker
    AP_INIT_TAKE1("FcgidAccessChecker", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_TAKE1("FastCgiAccessChecker", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAccessCheckerAuthoritative
    AP_INIT_FLAG("FcgidAccessCheckerAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_FLAG("FastCgiAccessCheckerAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAuthenticator
    AP_INIT_TAKE1("FcgidAuthenticator", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_TAKE1("FastCgiAuthenticator", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAuthenticatorAuthoritative
    AP_INIT_FLAG("FcgidAuthenticatorAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_FLAG("FastCgiAuthenticatorAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAuthorizer
    AP_INIT_TAKE1("FcgidAuthorizer", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_TAKE1("FastCgiAuthorizer", set_bad_directive_take1, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    // FcgidAuthorizerAuthoritative
    AP_INIT_FLAG("FcgidAuthorizerAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c"),
    AP_INIT_FLAG("FastCgiAuthorizerAuthoritative", set_bad_directive_flag, NULL,
        /* ACCESS_CONF | */ OR_FILEINFO,
        "See mod_fcgid.c - deprecated"),

    { NULL }
};



// Our name tag.
module AP_MODULE_DECLARE_DATA secfcgid_module =
{
    STANDARD20_MODULE_STUFF,
    create_dir_conf, /* Per-directory configuration handler. */
    NULL,            /* Merge handler for per-directory configurations. */
    NULL,            /* Per-server configuration handler. */
    NULL,            /* Merge handler for per-server configurations. */
    directives,      /* Any directives we may have for httpd. */
    register_hooks   /* Our hook registering function. */
};



// Hook registration function.
static void register_hooks(apr_pool_t *pool)
{
    ap_hook_handler(secfcgid_handler, NULL, NULL, APR_HOOK_LAST);
}



// Our secfcgid web service handler.
static int secfcgid_handler(request_rec *r)
{
    secfcgid_config *conf = (secfcgid_config *) ap_get_module_config(r->per_dir_config, &secfcgid_module);

    if (!conf->bad_wrap && !conf->bad_directive) {
        return DECLINED;
    }

    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
        "mod_secfcgid: HTTP_INTERNAL_SERVER_ERROR - \"%s\"", r->unparsed_uri);

    return HTTP_INTERNAL_SERVER_ERROR;
}



// Handler for the "FcgidWrapper" and "FcgiWrapper" directive.
// Checks every directive.
const char *set_check_path_take123(cmd_parms *cmd, void *config, const char *arg1, const char *arg2, const char *arg3)
{
    secfcgid_config *conf = (secfcgid_config *) config;

    if (conf) {
        // Path or part of the path to the wrappers.
        // Default directory for wrappers or edit this.
        const char *p_ = "/usr/fcgid-wrapper/";
        // strpos in C
        char *result = strstr(arg1, p_);
        if (result == NULL || (result - arg1) > 0) {
            conf->bad_wrap = 1;
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
                "mod_secfcgid: bad path to the wrapper - FcgidWrapper \"%s\"", arg1);
        } else {
            conf->bad_wrap = 0;
        }
        // If "p_" is array paths.
        /*
        const char *p_ = {
            "/usr/fcgid-wrapper/"
        }
        int len = sizeof p_ / sizeof *p_;
        for (int i = 0; i < len; i++) {
            // strpos in C
            char *result = strstr(arg1, p_[i]);
            if (result == NULL || (result - arg1) > 0) {
                conf->bad_wrap = 1;
            } else {
                conf->bad_wrap = 0;
            }
        }
        if (conf->bad_wrap) {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
            "mod_secfcgid: bad path to the wrapper - FcgidWrapper \"%s\"", arg1);
        }
        */
    }

    return NULL;
}



// Handler for the "FcgidAccessChecker, FastCgiAccessChecker,
// FcgidAuthenticator, FastCgiAuthenticator, FcgidAuthorizer, FastCgiAuthorizer" directives.
const char *set_bad_directive_take1(cmd_parms *cmd, void *config, const char *arg)
{
    secfcgid_config *conf = (secfcgid_config *) config;

    if (conf) {
        conf->bad_directive = 1;
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
            "mod_secfcgid: bad directive Fcgid.... \"%s\".", arg);
    }

    return NULL;
}



// Handler for the "FcgidAccessCheckerAuthoritative, FastCgiAccessCheckerAuthoritative,
// FcgidAuthenticatorAuthoritative, FastCgiAuthenticatorAuthoritative,
// FcgidAuthorizerAuthoritative, FastCgiAuthorizerAuthoritative" directives.
const char *set_bad_directive_flag(cmd_parms *cmd, void *config, int arg)
{
    secfcgid_config *conf = (secfcgid_config *) config;

    if (conf) {
        conf->bad_directive = 1;
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
            "mod_secfcgid: bad directive Fcgid.... \"%d\".", arg);
    }

    return NULL;
}



// Function for creating new configurations for per-directory contexts.
void *create_dir_conf(apr_pool_t *pool, char *context)
{
    secfcgid_config *conf = apr_pcalloc(pool, sizeof(secfcgid_config));

    if (conf) {
        // Set some default values.
        conf->bad_wrap = 0;
        conf->bad_directive = 0;
    }

    return conf;
}

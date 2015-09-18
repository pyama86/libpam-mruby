#include "mruby.h"

#include <string.h>
#include <sys/file.h>
#include <syslog.h>

#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv )
{
  return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags,int argc, const char **argv)
{
  int i;
  const char *username;
  const char *password;
  const char *rbfile_name = NULL;
  int debug = 0;

  for (i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "rbfile") && i + 1 < argc)
      rbfile_name = argv[++i];
    else if (!strncmp(argv[i], "rbfile=", strlen("rbfile=")))
      rbfile_name = argv[i] + strlen("rbfile=");
    else if (!strcmp(argv[i], "debug"))
      debug = 1;
  }

  // Check rbfile_name
  if (!rbfile_name) {
    pam_syslog(pamh, LOG_ERR, "mruby file name not specified");
    return PAM_AUTHINFO_UNAVAIL;
  }
  if (debug) pam_syslog(pamh, LOG_DEBUG, "rbfile is %s", rbfile_name);

  // Check user on PAM stack
  if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS) {
    pam_syslog(pamh, LOG_ERR, "couldn't get username from PAM stack");
    return PAM_AUTH_ERR;
  }
  if (debug) pam_syslog(pamh, LOG_DEBUG, "username is %s", username);

  return PAM_SUCCESS;
}

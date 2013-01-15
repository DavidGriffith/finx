/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMPSH, NX protocol compression, NX extensions to this software      */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rights reserved.                                                   */
/*                                                                        */
/**************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "Request.h"

Request::Request()
{
  logTrace("Request::Request");

  url_ = NULL;

  file_       = NULL;
  program_    = NULL;
  parameters_ = NULL;

  match_  = NULL;
  buffer_ = NULL;

  complete_ = 0;

  error_ = request_error_not_complete;
  type_  = request_type_unknown;
  mime_  = request_mime_unknown;

  rootPath_    = NULL;
  programPath_ = NULL;
  programName_ = NULL;
}

Request::~Request()
{
  logTrace("Request::~Request");

  delete [] url_;

  delete [] file_;
  delete [] program_;
  delete [] parameters_;

  delete [] match_;
  delete [] buffer_;

  delete [] rootPath_;
  delete [] programPath_;
  delete [] programName_;
}

const char *Request::getErrorString()
{
  switch (error_)
  {
    case request_error_not_complete:
    {
      return "Request Not Complete";
    }
    case request_error_none:
    {
      return "No Error";
    }
    case request_error_lenght_exceeded:
    {
      return "Request Length Exceeded";
    }
    case request_error_out_of_memory:
    {
      return "Out of Memory";
    }
    case request_error_no_trailing_newline:
    {
      return "No Trailing Newline";
    }
    case request_error_no_path_found:
    {
      return "No Path Found";
    }
    case request_error_no_get_found:
    {
      return "No Get Command Found";
    }
    case request_error_invalid_url_encode:
    {
      return "Invalid Url Encode";
    }
    case request_error_invalid_path:
    {
      return "Invalid Path";
    }
    case request_error_invalid_relative_path:
    {
      return "Invalid Relative Path";
    }
    case request_error_invalid_file_name:
    {
      return "Invalid File Name";
    }
    case request_error_url_not_printable:
    {
      return "Url Not Printable";
    }
    case request_error_file_not_found:
    {
      return "File Not Found";
    }
    case request_error_permission_denied:
    {
      return "Permission Denied";
    }
    case request_error_mime_unrecognized:
    {
      return "File Type Not Recognized";
    }
    default:
    {
      return "Unknown Error";
    }
  }
}

int Request::setValue(char *&recipient, const char *value)
{
  logTrace("Request::setValue");

  if (recipient != NULL)
  {
    logError("Request::setValue", ESET(EPERM));

    return -1;
  }
  else if ((recipient = new char[strlen(value) + 1]) == NULL)
  {
    logError("Request::setValue", ESET(ENOMEM));

    return -1;
  }

  strcpy(recipient, value);

  return 1;
}

int Request::parse(char *string, int length)
{
  logTrace("Request::parse");

  if (error_ != request_error_not_complete)
  {
    return -1;
  }
  else if (length >= bufferLimit_)
  {
    logError("Request::parse", ESET(EINVAL));

    error_ = request_error_lenght_exceeded;

    return -1;
  }

  //
  // Find and remove the trailing newline
  // and carriage return.
  //

  char *found;

  logTest("Request::parse", "Length is %d", length);

  if ((found = index(string, '\n')) == NULL)
  {
    logError("Request::parse", ESET(EINVAL));

    error_ = request_error_no_trailing_newline;

    return -1;
  }

  *found = '\0';

  length = found - string;

  logTest("Request::parse", "Length is now %d", length);

  if ((found = index(string, '\r')) != NULL)
  {
    *found = '\0';

    length = found - string;

    logTest("Request::parse", "Length is now %d", length);
  }

  logTest("Request::parse", "Parsing the string '%s'",
              string);

  //
  // If it is an empty string then go
  // validating the URL.
  //

  if (length == 0)
  {
    logTest("Request::parse", "Request is complete");

    error_ = request_error_none;

    return validate();
  }

  //
  // Find the 'Get' operation in the
  // request.
  //

  int result;

  if ((result = parseGetCommand(string)) == -1)
  {
    return result;
  }

  #ifdef TEST

  if (result == 0)
  {
    logTest("Request::parse", "Ignoring string '%s'",
                string);
  }

  #endif

  return 0;
}

int Request::parseGetCommand(char *&string)
{
  logTrace("Request::parseGetCommand");

  if (match_ == NULL)
  {
    match_ = new char[matchLimit_ + 1];
  }

  if (buffer_ == NULL)
  {
    buffer_ = new char[bufferLimit_ + 1];
  }

  if (match_ == NULL || buffer_ == NULL)
  {
    logError("Request::parseGetCommand", ESET(ENOMEM));

    error_ = request_error_out_of_memory;

    return -1;
  }

  //
  // Look for the 'Get' parameter.
  //

  int result;

  if (url_ == NULL &&
          strncmp(string, "get ", 4) == 0 ||
              strncmp(string, "GET ", 4) == 0 ||
                  strncmp(string, "Get ", 4) == 0)
  {
    snprintf(match_, matchLimit_, "%%%d[^ ] ", bufferLimit_);

    logTest("Request::parseGetCommand", "Trying to match pattern '%s'",
                match_);

    result = sscanf(string + 4, match_, buffer_);

    if (result == 1)
    {
      logTest("Request::parseGetCommand", "Matched 'Get' with value '%s'",
                  buffer_);

      url_ = new char[strlen(buffer_) + 1];

      if (url_ == NULL)
      {
        logError("Request::parseGetCommand", ESET(ENOMEM));

        error_ = request_error_out_of_memory;

        return -1;
      }

      strcpy(url_, buffer_);

      type_ = request_type_get;
    }
  }

  return 1;
}

int Request::validate()
{
  logTrace("Request::validate");

  if (error_ != request_error_none)
  {
    return -1;
  }
  else if (rootPath_ == NULL || programPath_ == NULL)
  {
    logTest("Request::validate", "Can't find the root "
                "for program or file");

    logError("Request::validate", ESET(EINVAL));

    error_ = request_error_no_path_found;

    return -1;
  }
  else if (url_ == NULL)
  {
    logTest("Request::validate", "Can't find a 'Get' "
                "parameter");

    logError("Request::validate", ESET(EINVAL));

    error_ = request_error_no_get_found;

    return -1;
  }
  //
  // Now use the buffer as a temporary.
  //

  strcpy(buffer_, url_);

  //
  // Remove the trailing URL parameters.
  //

  int result;

  if ((result = validateUrlParameters(buffer_)) == -1)
  {
    return result;
  }

  //
  // Decode the URL encoded characters.
  //

  if ((result = validateUrlEncoded(buffer_)) == -1)
  {
    return result;
  }

  //
  // Ensure that the file name doesn't contain
  // references to a relative path or a file
  // name starting with a dot.
  //

  if ((result = validateUrlPath(buffer_)) == -1)
  {
    return result;
  }

  //
  // Ensure that the file name only contains
  // printable characters.
  //

  if ((result = validateUrlPrintable(buffer_)) == -1)
  {
    return result;
  }

  //
  // Check if the URL is a file or a program and
  // if the request is valid based on our root.
  //

  if ((result = validateProgramPath(buffer_)) == -1)
  {
    return result;
  }

  if (result == 0 &&
          (result = validateFilePath(buffer_)) == -1)
  {
    return result;
  }

  if (result == 1)
  {
    #ifdef TEST

    logTest("Request::validate", "Matched a valid URL "
                "'%s'", url_);

    if (program_ != NULL)
    {
      logTest("Request::validate", "URL is CGI "
                  "program '%s'", program_);

      if (parameters_ != NULL)
      {
        logTest("Request::validate", "Parameters are "
                    "'%s'", parameters_);
      }
    }
    else
    {
      logTest("Request::validate", "URL is local "
                  "file '%s'", file_);

      logTest("Request::validate", "Content type "
                  "is '%s'", getMimeString());
    }

    #endif

    complete_ = 1;

    return 1;
  }

  logError("Request::validate", ESET(EINVAL));

  error_ = request_error_mime_unrecognized;

  return -1;
}

int Request::validateUrlParameters(char *&string)
{
  logTrace("Request::validateUrlParameters");

  char *found;

  if ((found = index(string, '?')) != NULL)
  {
    logTest("Request::validateUrlParameters", "Removing "
                "the URL parameters");

    *found++ = '\0';

    if (*found != '\0')
    {
      parameters_ = new char[strlen(found) + 1];

      if (parameters_ == NULL)
      {
        logError("Request::validateUrlParameters", ESET(ENOMEM));

        error_ = request_error_out_of_memory;

        return -1;
      }

      strcpy(parameters_, found);

      logTest("Request::validateUrlParameters", "Got URL "
                  "parameters '%s'", parameters_);
    }
  }

  logTest("Request::validateUrlParameters", "Buffer is "
              "now '%s'", string);

  return 0;
}

int Request::validateUrlEncoded(char *&string)
{
  logTrace("Request::validateUrlEncoded");

  char *found;

  char *start = string;

  unsigned int value[2];

  int result;

  while ((found = index(start, '%')) != NULL)
  {
    if ((result = sscanf(found, "%%%1x%1x",
            &value[0], &value[1])) != 2)
    {
      logError("Request::validateUrlEncoded", ESET(EINVAL));

      error_ = request_error_invalid_url_encode;

      return -1;
    }

    value[0] = value[0] * 16 + value[1];

    logTest("Request::validateUrlEncoded", "Got URL encoded "
                "value 0x%x decimal %d", value[0], value[0]);

    *found = value[0];

    memmove(found + 1, found + 3, strlen(found) - 2);

    start = found + 1;

    logTest("Request::validateUrlEncoded", "Buffer is "
                "now '%s'", string);
  }

  return 0;
}

int Request::validateUrlPath(char *&string)
{
  logTrace("Request::validateUrlPath");

  if (*buffer_ != '/')
  {
    logError("Request::validateUrlPath", ESET(EINVAL));

    error_ = request_error_invalid_path;

    return -1;
  }

  char *found;

  char *start = string;

  while ((found = index(start, '/')) != NULL)
  {
    if (*(found + 1) == '.')
    {
      logError("Request::validateUrlPath", ESET(EINVAL));

      if (*(found + 2) == '\0' ||
              *(found + 2) == '.' ||
                  *(found + 2) == '/')
      {
        error_ = request_error_invalid_relative_path;
      }
      else
      {
        error_ = request_error_invalid_file_name;
      }

      return -1;
    }

    start = found + 1;
  }

  return 0;
}

int Request::validateUrlPrintable(char *&string)
{
  logTrace("Request::validateUrlPrintable");

  char *start = string;

  while (*start != '\0')
  {
    if (isprint(*start++) == 0)
    {
      logError("Request::validateUrlPrintable", ESET(EINVAL));

      error_ = request_error_url_not_printable;

      return -1;
    }
  }

  return 0;
}

int Request::validateProgramPath(char *&string)
{
  logTrace("Request::validateProgramPath");

  if (strcmp(string + 1, programName_) == 0)
  {
    if ((program_ = new char[strlen(programPath_) +
            strlen(string) + 1]) == NULL)
    {
      logError("Request::validateProgramPath", ESET(ENOMEM));

      error_ = request_error_out_of_memory;

      return -1;
    }

    strcpy(program_, programPath_);
    strcat(program_, string);

    logTest("Request::validateProgramPath", "Got CGI "
                "program '%s'", program_);

    mime_ = request_mime_program;

    return 1;
  }

  return 0;
}

int Request::validateFilePath(char *&string)
{
  logTrace("Request::validateFilePath");

  //
  // Assume that the file is a file with the
  // given name under the root path.
  //

  if ((file_ = new char[strlen(rootPath_) +
            strlen(string) + 2]) == NULL)
  {
    logError("Request::validateFilePath", ESET(ENOMEM));

    error_ = request_error_out_of_memory;

    return -1;
  }

  strcpy(file_, rootPath_);
  strcat(file_, string);

  logTest("Request::validateFilePath", "Got "
              "file '%s'", file_);

  if (parameters_ != NULL)
  {
    logTest("Request::validateFilePath", "Will ignore "
                "parameters '%s'", parameters_);

    delete [] parameters_;

    parameters_ = NULL;
  }

  //
  // Check if the file exists and it is a
  // regular file.
  //

  struct stat info;

  if (stat(file_, &info) != 0)
  {
    logTest("Request::validateFilePath", "Can't get info "
                "about the file '%s'", file_);

    logError("Request::validateFilePath::stat", EGET());

    error_ = request_error_file_not_found;

    return -1;
  }
  else if (S_ISREG(info.st_mode) == 0)
  {
    logError("Request::validateFilePath", ESET(EPERM));

    error_ = request_error_permission_denied;

    return -1;
  }

  //
  // Check if the mime is supported.
  //

  int length = strlen(file_);

  if (checkMime(file_, length, ".html") == 1 ||
          checkMime(file_, length, ".htm") == 1)
  {
    mime_ = request_mime_text_html;

    return 1;
  }
  else if (checkMime(file_, length, ".css") == 1)
  {
    mime_ = request_mime_text_css;

    return 1;
  }
  else if (checkMime(file_, length, ".png") == 1)
  {
    mime_ = request_mime_image_png;

    return 1;
  }
  else if (checkMime(file_, length, ".gif") == 1)
  {
    mime_ = request_mime_image_gif;

    return 1;
  }
  else if (checkMime(file_, length, ".ico") == 1)
  {
    mime_ = request_mime_image_ico;

    return 1;
  }
  else if (checkMime(file_, length, ".js") == 1)
  {
    mime_ = request_mime_javascript;

    return 1;
  }

/*
FIXME

  //
  // Return the file as plain text but only
  // if execute permissions are off.
  //

  if ((info.st_mode & S_IXUSR) == 0 &&
          (info.st_mode & S_IXGRP) == 0 &&
              (info.st_mode & S_IXOTH) == 0)
  {
    mime_ = request_mime_file_text_plain;

    return 1;
  }
*/

  return 0;
}

int Request::checkMime(const char *file, const int length,
                           const char *mime)
{
  logTrace("Request::checkMime");

  int match = strlen(mime);

  if (length > match && strcmp(file + length - match, mime) == 0)
  {
    return 1;
  }

  return 0;
}

const char *Request::getMimeString()
{
  logTrace("Request::getMimeString");

  switch (mime_)
  {
    case request_mime_text_html:
    {
      return "text/html";
    }
    case request_mime_text_plain:
    {
      return "text/plain";
    }
    case request_mime_text_css:
    {
      return "text/css";
    }
    case request_mime_image_png:
    {
      return "image/png";
    }
    case request_mime_image_gif:
    {
      return "image/gif";
    }
    case request_mime_image_ico:
    {
      return "text/plain";
    }
    case request_mime_javascript:
    {
      return "application/x-javascript";
    }
    default:
    {
      return "";
    }
  }
}

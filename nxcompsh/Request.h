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

#ifndef Request_H
#define Request_H

enum T_request_error
{
  request_error_not_complete = -1,
  request_error_none = 0,
  request_error_lenght_exceeded,
  request_error_out_of_memory,
  request_error_no_trailing_newline,
  request_error_no_path_found,
  request_error_no_get_found,
  request_error_invalid_url_encode,
  request_error_invalid_path,
  request_error_invalid_relative_path,
  request_error_invalid_file_name,
  request_error_url_not_printable,
  request_error_file_not_found,
  request_error_permission_denied,
  request_error_mime_unrecognized
};

enum T_request_type
{
  request_type_unknown = -1,
  request_type_get = 0,
  request_type_post
};

enum T_request_mime
{
  request_mime_unknown = -1,
  request_mime_program = 0,
  request_mime_text_html,
  request_mime_text_plain,
  request_mime_text_css,
  request_mime_image_png,
  request_mime_image_gif,
  request_mime_image_ico,
  request_mime_javascript
};

class Request
{
  public:

  Request();

  ~Request();

  //
  // The real root of the files served
  // by the HTTPD server.
  //

  int setRootPath(const char *root)
  {
    return setValue(rootPath_, root);
  }

  //
  // The binary program that is allowed
  // to run as a CGI.
  //

  int setProgramPath(const char *path)
  {
    return setValue(programPath_, path);
  }

  int setProgramName(const char *name)
  {
    return setValue(programName_, name);
  }

  int isComplete()
  {
    return complete_;
  }

  int isError()
  {
    return (error_ != request_error_none);
  }

  int isFile()
  {
    return (complete_ == 1 && error_ == request_error_none ?
                file_ != NULL : -1);
  }

  int isProgram()
  {
    return (complete_ == 1 && error_ == request_error_none ?
                program_ != NULL : -1);
  }

  T_request_error getError()
  {
    return (complete_ == 1 ? error_ : request_error_not_complete);
  }

  const char *getErrorString();

  const char *getUrl()
  {
    return (complete_ == 1 && error_ == request_error_none ?
                url_ : NULL);
  }

  const char *getFile()
  {
    return (complete_ == 1 && error_ == request_error_none ?
                file_ : NULL);
  }

  const char *getProgram()
  {
    return (complete_ == 1 && error_ == request_error_none ?
                program_ : NULL);
  }

  T_request_type getRequestType()
  {
    return type_;
  }

  T_request_mime getMimeType()
  {
    return mime_;
  }

  const char *getMimeString();

  int parse(char *line, int length);

  protected:

  int validate();

  private:

  int parseGetCommand(char *&string);

  int validateUrlParameters(char *&string);
  int validateUrlEncoded(char *&string);
  int validateUrlPath(char *&string);
  int validateUrlPrintable(char *&string);

  int validateProgramPath(char *&string);
  int validateFilePath(char *&string);

  int setValue(char *&recipient, const char *value);

  int checkMime(const char *file, const int length,
                    const char *mime);

  char *url_;

  char *file_;
  char *program_;
  char *parameters_;

  int complete_;

  static const int matchLimit_  = 1024;
  static const int bufferLimit_ = 16384;

  char *match_;
  char *buffer_;

  char *rootPath_;
  char *programPath_;
  char *programName_;

  T_request_error error_;
  T_request_type  type_;
  T_request_mime  mime_;
};

#endif /* Request_H */

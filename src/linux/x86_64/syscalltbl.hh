static std::map<long, SystemCallSignature> syscall_signatures =
{
  {
    SYS_open,
    {
      SYS_open, "open",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* pathname */
        { typeid( int ),    ARGUMENT_DIR_IN, 0 }, /* flags */
      }
    }
  },
  {
    SYS_execve,
    {
      SYS_execve, "execve",
      {
        { typeid( char * ), ARGUMENT_DIR_IN, ARGUMENT_F_PATHNAME }, /* filename */
        /* there are other arguments as well */
      }
    }
  }
};

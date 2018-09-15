/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "status_bar.hh"

#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#define HIDE_CURSOR "\033[25l"
#define SHOW_CURSOR "\033[25h"

StatusBar::StatusBar()
{
  init();
}

StatusBar::~StatusBar()
{
  remove();
}

StatusBar & StatusBar::get()
{
  static StatusBar status_bar;
  return status_bar;
}

void StatusBar::init()
{
  ioctl( STDOUT_FILENO, TIOCGWINSZ, &window_size_ );

  ostringstream oss;
  oss << "\n\0337\033[1;" << window_size_.ws_row - 1 << "r\033["
      << window_size_.ws_row << ";1H\033[2K\0338\033[1A";

  cerr << oss.str() << flush;
}

void StatusBar::remove()
{
  set_text( "" );
  cerr << "\0337\033[1;" << window_size_.ws_row << "r\0338" << flush;
}

void StatusBar::redraw()
{
  get().remove();
  get().init();
  get().set_text( get().text_ );
}

void StatusBar::set_text( const string & text )
{
  StatusBar & status_bar = get();
  status_bar.text_ = text;

  ostringstream oss;
  oss << HIDE_CURSOR
      << "\0337\033[" << status_bar.window_size_.ws_row << ";1H\033[K"
      << status_bar.text_
      << "\033[K\0338"
      << SHOW_CURSOR;

  cerr << oss.str() << flush;
}

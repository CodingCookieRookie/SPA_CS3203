#pragma once

#ifndef PARENTT_H
#define PARENTT_H

#include "./RS2T.h"
#include "./Parent.h"

class ParentT : public RS2T<ParentT, Parent> {};

#endif

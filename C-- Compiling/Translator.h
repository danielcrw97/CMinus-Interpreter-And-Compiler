#pragma once

class IRContainer;

// Translate three address code representation to MIPS assembly.
void translate(IRContainer& ir);
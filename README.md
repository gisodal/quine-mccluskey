# Quine-McCluskey
The Quine-McCluskey Algorithm in C++, using pthreads.

## Synopsis

This is a parallel implementation of the Quine-McCluskey algorithm, which produces prime implicants in sum-of-product (SOP) form. Patrick's method is used to obtain the canonical SOP form.

## Usage

> bin/quine-mccluskey -v <#variables> -o <#model1[,#model2[,...]]>

For example:
> bin/quine-mccluskey -v3 -o0,2,5,7

((¬a ∧ ¬c)  ∨  (a ∧ c))

## Installation

Just type:
> make

for other options, type:
> make help


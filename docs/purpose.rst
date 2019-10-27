The purposes of this document
================================================================

Halide already has an excellent
`tutorial<https://halide-lang.org/tutorials/tutorial_introduction.html>`_ that
explains the syntax of the language and the basics of scheduling in detail.
This document is an alternative to the official tutorial, that serves several
complementary purposes:

- We provide a more lightweight introduction to Halide emphasizing what it can
  do and how to do it, with less focus on the scheduling language. We assume
  the autoscheduler can provide an efficient schedule.

- We show real Halide code examples and explain them, so that the readers can
  have a better understanding of Halide's capabilities. We also briefly discuss
  strategies for manual scheduling.

- We show how to incorporate learning components in Halide programs with
  automatic differentiation, and how Halide interacts with deep learning
  frameworks such as PyTorch.

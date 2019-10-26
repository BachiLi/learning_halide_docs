The purposes of this document
================================================================

Halide already has an excellent `tutorial <https://halide-lang.org/tutorials/tutorial_introduction.html>`_ that detailedly explains the syntax of the language and the basics of scheduling. This document is an alternative to the official tutorial, that serves several purposes:

- We provide a more lightweight introduction to Halide about what it can do and how to do it, with less focus on the scheduling language, assuming the autoscheduler can provide an efficient schedule.

- We show real Halide code examples and provides explanation on them, so that the readers can have a better understanding of Halide's capability. We also briefly discuss the strategy for manually scheduling the pipeline.

- We show how to incorporate learning components in Halide programs with automatic differentiation, and how Halide interacts with deep learning frameworks such as PyTorch.

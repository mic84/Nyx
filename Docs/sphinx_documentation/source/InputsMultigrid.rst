.. _Chap:InputsMultigrid:

Multigrid Inputs
================

The following inputs can be set directly in the AMReX solver classes but we set them via the Nyx gravity routines.

These must be preceded by "gravity" in the inputs file:

+----------------------+-----------------------------------------------------------------------+-------------+--------------+
|                      | Description                                                           |   Type      | Default      |
+======================+=======================================================================+=============+==============+
| v                    |  Verbosity of multigrid solver                                        |    Int      |   0          |
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
| ml_tol               |  Relative tolerance for multilevel solves                             |    Real     |   1.e-12     | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
| sl_tol               |  Relative tolerance for single-level solves                           |    Real     |   1.e-12     | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
| mlmg_max_fmg_iter    |  Maximum number of FMG cycles before starting V-cycles                |    Int      |   0          | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
| mlmg_agglomeration   |  Should we agglomerate deep in the V-cycle                            |    Int      |   0          | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
| mlmg_consolidation   |  Should we consolidate deep in the V-cycle                            |    Int      |   0          | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+

There are additional multigrid parameters for which Nyx just uses the AMReX default -- these include 

+----------------------+-----------------------------------------------------------------------+-------------+--------------+
|                      | Description                                                           |   Type      | Default      |
+======================+=======================================================================+=============+==============+
|                      |  Verbosity of BiCG bottom solver                                      |    Int      |   0          |
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
|                      |  Absolute tolerance for multilevel solves                             |    Real     |   0.         | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
|                      |  Maximum number of V-cycles                                           |    Int      |   200        | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+
|                      |  Maximum number of BiCG iterations in bottom solve                    |    Int      |   100        | 
+----------------------+-----------------------------------------------------------------------+-------------+--------------+

# AMREX_HOME defines the directory in which we will find all the AMReX code
AMREX_HOME ?= ../../../amrex

CVODE_LIB_DIR ?= $(CVODE_LIB)

# TOP defines the directory in which we will find Source, Exec, etc
TOP = ../..

# compilation options
COMP    = intel # gnu
USE_MPI = TRUE
USE_OMP = TRUE

USE_SENSEI_INSITU = FALSE
USE_HENSON = FALSE

PROFILE       = TRUE
TRACE_PROFILE = FALSE
COMM_PROFILE  = FALSE
TINY_PROFILE  = FALSE

PRECISION = DOUBLE
USE_SINGLE_PRECISION_PARTICLES = TRUE
DEBUG     = FALSE

GIMLET = FALSE
REEBER = FALSE

# physics
DIM      = 3
USE_GRAV = TRUE
USE_HEATCOOL = TRUE
USE_AGN = FALSE
USE_CVODE = FALSE
USE_SUNDIALS3 = FALSE
NEUTRINO_PARTICLES=TRUE
NEUTRINO_DARK_PARTICLES=TRUE

Bpack := ./Make.package
Blocs := .

include $(TOP)/Exec/Make.Nyx

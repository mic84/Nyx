#include <AMReX_REAL.H>

#include <Derive.H>
#include <Nyx.H>

using namespace amrex;

#ifdef __cplusplus
extern "C"
{
#endif

  void derstate(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                   const FArrayBox& datfab, const Geometry& geomdata,
                   Real /*time*/, const int* /*bcrec*/, int /*level*/)
  {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        // density
        der(i,j,k,0) = dat(i,j,k,0);

        // temperature
        der(i,j,k,1) = dat(i,j,k,1);

        if(der.nComp()>=3 && dat.nComp()>=3)
        {
            // (rho X)_1 = X_1
            der(i,j,k,2) = dat(i,j,k,2) / dat(i,j,k,0);
        }

      });

    }

    void dervel(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                   const FArrayBox& datfab, const Geometry& geomdata,
                   Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {
             der(i,j,k,0) = dat(i,j,k,1) / dat(i,j,k,0);
      });
    }

    void dermagvel(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                   const FArrayBox& datfab, const Geometry& geomdata,
                   Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = std::sqrt( (dat(i,j,k,1) / dat(i,j,k,0))*(dat(i,j,k,1) / dat(i,j,k,0)) +
                                  (dat(i,j,k,2) / dat(i,j,k,0))*(dat(i,j,k,2) / dat(i,j,k,0)) +
                                  (dat(i,j,k,3) / dat(i,j,k,0))*(dat(i,j,k,3) / dat(i,j,k,0)));
      });
    }

    void dermagvort(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                    const FArrayBox& datfab, const Geometry& geomdata,
                    Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        Real uy = (dat(i,j+1,k,1) - dat(i,j-1,k,1)) / dx[1];
        Real uz = (dat(i,j,k+1,1) - dat(i,j,k-1,1)) / dx[2];
        Real vx = (dat(i+1,j,k,2) - dat(i-1,j,k,2)) / dx[0];
        Real vz = (dat(i,j,k+1,2) - dat(i,j,k-1,2)) / dx[2];
        Real wx = (dat(i+1,j,k,3) - dat(i-1,j,k,3)) / dx[0];
        Real wy = (dat(i,j+1,k,3) - dat(i,j-1,k,3)) / dx[1];
 
        Real v1 = 0.5 * amrex::Math::abs(wy - vz);
        Real v2 = 0.5 * amrex::Math::abs(uz - wx);
        Real v3 = 0.5 * amrex::Math::abs(vx - uy);

        der(i,j,k,0) = std::sqrt(v1*v1 + v2*v2 + v3*v3); 
      });
    }

    void dermaggrav(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                    const FArrayBox& datfab, const Geometry& geomdata,
                    Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = std::sqrt(dat(i,j,k,0)*dat(i,j,k,0) +
                                 dat(i,j,k,1)*dat(i,j,k,1) +
                                 dat(i,j,k,2)*dat(i,j,k,2));

      });
    }

  void dermagmom(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                    const FArrayBox& datfab, const Geometry& geomdata,
                    Real /*time*/, const int* /*bcrec*/, int /*level*/)
  {

    auto const dat = datfab.array();
    auto const der = derfab.array();

    amrex::ParallelFor(bx,
                       [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
                       {

                         der(i,j,k,0) = std::sqrt(dat(i,j,k,0)*dat(i,j,k,0) +
                                                  dat(i,j,k,1)*dat(i,j,k,1) +
                                                  dat(i,j,k,2)*dat(i,j,k,2));

                       });
  }

    void derpres(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                 const FArrayBox& datfab, const Geometry& geomdata,
                 Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::Real gamma_minus_1_local = Nyx::gamma - 1.0_rt;
      //No abort call for non-positive energy
      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = gamma_minus_1_local * dat(i,j,k,Eint);

      });
    }

    void derlogden(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                   const FArrayBox& datfab, const Geometry& geomdata,
                   Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {
             der(i,j,k,0) = std::log10(dat(i,j,k,0));
      });
    }

    void dereint1(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                  const FArrayBox& datfab, const Geometry& geomdata,
                  Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        Real rhoInv = 1.0_rt/dat(i,j,k,Density);
        Real ux = dat(i,j,k,Xmom)*rhoInv;
        Real uy = dat(i,j,k,Ymom)*rhoInv;
        Real uz = dat(i,j,k,Zmom)*rhoInv;

        der(i,j,k,0) = dat(i,j,k,Eden)*rhoInv -
          0.5_rt * (ux*ux + uy*uy + uz*uz);
      });
    }

    void dereint2(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                  const FArrayBox& datfab, const Geometry& geomdata,
                  Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = dat(i,j,k,Eint) / dat(i,j,k,Density);
      });
    }


    void dersoundspeed(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                       const FArrayBox& datfab, const Geometry& geomdata,
                       Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      // Here dat contains the full conserved state variable

      Real sound_speed_factor=sqrt(Nyx::gamma*(Nyx::gamma-1.0_rt));

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        Real rhoInv = 1.0_rt/dat(i,j,k,Density);
        Real ux = dat(i,j,k,Xmom)*rhoInv;
        Real uy = dat(i,j,k,Ymom)*rhoInv;
        Real uz = dat(i,j,k,Zmom)*rhoInv;

        // Use internal energy for calculating dt 
        Real e  = dat(i,j,k,Eint)*rhoInv;

        // Protect against negative e
#ifdef HEATCOOL
        if (e > 0.0)
            der(i,j,k,0)=sound_speed_factor*std::sqrt(e);
#else
        if (e > 0.0)
            der(i,j,k,0)=sound_speed_factor*std::sqrt(dat(i,j,k,Density)*e/dat(i,j,k,Density));
#endif
        else
            der(i,j,k,0) = 0.0;

      });
    }

    void derentropy(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                 const FArrayBox& datfab, const Geometry& geomdata,
                 Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      //  Here dat contains (Density, Xmom, Ymom, Zmom, (rho E), (rho e), Temp, Ne)

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {
           Real rhoInv = 1.0_rt/dat(i,j,k,Density);
           Real      e = dat(i,j,k,Eint)*rhoInv;

        // Protect against negative e
#ifdef HEATCOOL
//      if (e > 0.0)
//            call nyx_eos_S_given_Re(s(i,j,k,1), u(i,j,k,URHO), e, &
//                                    u(i,j,k,7), u(i,j,k,8), &
//                                    comoving_a = 1.d0)
#else
//      if (e > 0.0)
//            call nyx_eos_S_given_Re(s(i,j,k,1), u(i,j,k,URHO), e, &
//                                    u(i,j,k,7), u(i,j,k,8), &
//                                    comoving_a = 1.d0)
#endif
//      else
            der(i,j,k,0) = 0.0;
      });
    }

    void dermachnumber(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                       const FArrayBox& datfab, const Geometry& geomdata,
                       Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      // Here dat contains the full conserved state variable

      Real sound_speed_factor=sqrt(Nyx::gamma*(Nyx::gamma-1.0_rt));

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        Real rhoInv = 1.0_rt/dat(i,j,k,Density);
        Real ux = dat(i,j,k,Xmom)*rhoInv;
        Real uy = dat(i,j,k,Ymom)*rhoInv;
        Real uz = dat(i,j,k,Zmom)*rhoInv;

        // Use internal energy for calculating dt 
        Real e  = dat(i,j,k,Eint)*rhoInv;

        Real c;
        // Protect against negative e
#ifdef HEATCOOL
        if (e > 0.0)
                c = sound_speed_factor*std::sqrt(e);
#else
        if (e > 0.0)
            c=sound_speed_factor*std::sqrt(dat(i,j,k,Density)*e/dat(i,j,k,Density));
#endif
        else
            c = 0.0;

        der(i,j,k,0) = std::sqrt((ux*ux + uy*uy + uz*uz)) / c;

      });

    }

  void derkineng (const Box& bx, FArrayBox& kinengfab, int dcomp, int /*ncomp*/,
                     const FArrayBox& datfab, const Geometry& /*geomdata*/,
                     Real /*time*/, const int* /*bcrec*/, int /*level*/)
  {

    auto const dat = datfab.array();
    auto const kineng = kinengfab.array();

    amrex::ParallelFor(bx,
                       [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
                       {
                         kineng(i,j,k,0) = 0.5_rt / dat(i,j,k,0) * ( dat(i,j,k,1)*dat(i,j,k,1) +
                                                                     dat(i,j,k,2)*dat(i,j,k,2) +
                                                                     dat(i,j,k,3)*dat(i,j,k,3) );
                       });

  }

  void dernull(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
               const FArrayBox& datfab, const Geometry& geomdata,
               Real /*time*/, const int* /*bcrec*/, int /*level*/)
  {

    // This routine is used by particle_count.  Yes it does nothing.

  }

  void derspec(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
               const FArrayBox& datfab, const Geometry& geomdata,
               Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {
             der(i,j,k,0) = dat(i,j,k,1) / dat(i,j,k,0);
      });
    }

    void derdivu(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                 const FArrayBox& datfab, const Geometry& geomdata,
                 Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      // Here dat contains (Density, Xmom, Ymom, Zmom)

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        Real uhi = dat(i+1,j,k,1) / dat(i+1,j,k,0);
        Real ulo = dat(i-1,j,k,1) / dat(i-1,j,k,0);
        Real vhi = dat(i,j+1,k,2) / dat(i,j+1,k,0);
        Real vlo = dat(i,j-1,k,2) / dat(i,j-1,k,0);
        Real whi = dat(i,j,k+1,3) / dat(i,j,k+1,0);
        Real wlo = dat(i,j,k-1,3) / dat(i,j,k-1,0);

        der(i,j,k,0) = 0.5 * ( (uhi-ulo) / dx[0] + (vhi-vlo) / dx[1] + (whi-wlo) / dx[2] );

      });
    }

    void dermomt(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                 const FArrayBox& datfab, const Geometry& geomdata,
                 Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      // Here dat contains (Density, Single Component of Momentum, Sdens)

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

          der(i,j,k,0) = dat(i,j,k,1) + dat(i,j,k,1)*dat(i,j,k,2)/dat(i,j,k,0);

      });
    }

    void derdenvol(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                   const FArrayBox& datfab, const Geometry& geomdata,
                   Real /*time*/, const int* /*bcrec*/, int /*level*/)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      // Here dat contains (Density, Xmom, Ymom, Zmom)
      const Real V_cell = dx[0] * dx[1] * dx[2];
      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = V_cell * dat(i,j,k,Density);

      });
    }

    void deroverden(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                    const FArrayBox& datfab, const Geometry& geomdata,
                    Real /*time*/, const int* /*bcrec*/, int level)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      // Here dat contains (Density, Xmom, Ymom, Zmom)
      const Real over_den = Nyx::average_total_density * std::pow(8,level+1);

      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = dat(i,j,k,Density) / over_den;

      });
    }

    void deroverdenzoom(const Box& bx, FArrayBox& derfab, int dcomp, int /*ncomp*/,
                        const FArrayBox& datfab, const Geometry& geomdata,
                        Real /*time*/, const int* /*bcrec*/, int level)
    {

      auto const dat = datfab.array();
      auto const der = derfab.array();

      auto const dx = geomdata.CellSizeArray();

      //Assume Domain is a cube
      int idim = 0;
      int domlo = geomdata.Domain().smallEnd(idim);
      int domhi = geomdata.Domain().bigEnd(idim);

      int ref_size = domhi / (2*std::pow(2,(level+1)));
      int center   = (domhi-domlo+1) / 2;

      int ilo      = amrex::max(center-ref_size+1, bx.smallEnd(idim));
      int ihi      = amrex::min(center+ref_size,   bx.bigEnd(idim));
      auto const bx_ref = Box(IntVect(AMREX_D_DECL(amrex::max(center-ref_size+1, bx.smallEnd(0)),
                                                   amrex::max(center-ref_size+1, bx.smallEnd(1)),
                                                   amrex::max(center-ref_size+1, bx.smallEnd(2)))),
                              IntVect(AMREX_D_DECL(amrex::min(center+ref_size,   bx.bigEnd(0)),
                                                   amrex::min(center+ref_size,   bx.bigEnd(1)),
                                                   amrex::min(center+ref_size,   bx.bigEnd(2))) ));
      amrex::ParallelFor(bx,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = 0.0;

      });
      amrex::ParallelFor(bx_ref,
      [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept
      {

        der(i,j,k,0) = 1.0;

      });
      
    }

#ifdef __cplusplus
}
#endif

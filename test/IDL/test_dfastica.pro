@dfastica
@miv_load

PRO Test_DFastICA
  c1 = MIV_Load('testdata\c1.miv', DIMENSIONS=dim)
  c2 = MIV_Load('testdata\c2.miv')
  c3 = MIV_Load('testdata\c3.miv')
  ns = n_elements(c1)
  Sx = [reform(c3, 1, ns), reform(c1, 1, ns), reform(c2, 1, ns)]
  window, 0, XSIZE=2*dim[0], YSIZE=dim[1]
  tv, reform(Sx, 3, dim[0], dim[1]),   0, 0, /TRUE, /ORDER
  Ux = DFastICA_Unmix(Sx, MX=Mx)
  tvscl, reform(Ux, 3, dim[0], dim[1]), dim[0], 0, /TRUE, /ORDER
  print, Mx
END

PRO Test_Stack_DFastICA
  c1 = MIV_Load('testdata\s1.miv', DIMENSIONS=dim)
  c2 = MIV_Load('testdata\s2.miv')
  c3 = MIV_Load('testdata\s3.miv')
  ns = n_elements(c1)
  Sx = [reform(c3, 1, ns), reform(c1, 1, ns), reform(c2, 1, ns)]
  window, 0, XSIZE=2*dim[0], YSIZE=dim[1]
  tv, (reform(Sx, 3, dim[0], dim[1], dim[2]))[*,*,*,5],   0, 0, /TRUE, /ORDER
  Ux = DFastICA_Unmix(Sx, MX=Mx, /UNLOAD) ; add /UNLOAD to release the FastICA.dll after using
  Ux = reform(Ux, 3, dim[0], dim[1], dim[2])
  tvscl, Ux[*,*,*,5], dim[0], 0, /TRUE, /ORDER
  print, Mx
END
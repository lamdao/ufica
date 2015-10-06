FUNCTION DFastICA_Estimare, data, _EXTRA=e
  dim = size(data)
  nc = dim[1] gt dim[2] ? dim[2] : dim[1]
  mx = dblarr(nc, nc)
  rt = call_external('FastICA.dll', 'DEstimate', mx, data, dim[3], dim[1], dim[2], /I_VALUE, _EXTRA=e)
  return, mx
END

FUNCTION DFastICA_Unmix, data, MX=mx, _EXTRA=e
  data = uint(data) ; convert to 16 bit - unmix requires 16bit buffer for results
  dim = size(data)
  nc = dim[1] gt dim[2] ? dim[2] : dim[1]
  if n_elements(mx) eq 0 then mx = dblarr(nc, nc)
  rt = call_external('FastICA.dll', 'DUnmix', mx, data, dim[3], dim[1], dim[2], /I_VALUE, _EXTRA=e)
  return, data
END

PRO DFastICA_Unload
  rt = call_external('FastICA.dll', 'DUnload', /I_VALUE, /UNLOAD)
END
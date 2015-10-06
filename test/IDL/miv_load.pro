FUNCTION MIV_Load, fn, VOXEL_SIZE=vs, UNIT=un, DIMENSIONS=dim
  mivf = '    '
  openr, fp, fn, /GET_LUN
  readu, fp, mivf
  if mivf ne 'MIVF' then begin
    print, fn, ' is not Microscope Image Volume file format.'
    return, !NULL
  end

  dim = [0L, 0L, 0L]
  readu, fp, dim
  foreach d, dim do begin
    if d le 0 then begin
      print, fn, ' has incorrect dimensions: ', dim
      return, !NULL
    end
  end

  dtype = 0L
  readu, fp, dtype
  case dtype of
    0: vol = bytarr(dim)
    1: vol = intarr(dim)      ; 16bit signed
    2: vol = uintarr(dim)     ; 16bit unsigned
    3: vol = lonarr(dim)      ; 32bit signed
    4: vol = fltarr(dim)      ; 32bit float
    5: vol = bytarr(dim)      ; COLOR8
    6: vol = bytarr([3,dim])  ; RGB
   11: vol = ulonarr(dim)     ; 32bit unsigned
   12: vol = bytarr([4,dim])  ; RGB48
   13: vol = uintarr(dim)     ; 12bit unsigned
   16: vol = dblarr(dim)      ; 64bit float
   else: begin
      r = dialog_message('Unsupported data type '+strtrim(dt,2), /ERROR)
      return, !NULL
   end
  endcase
  
  un = 0L
  readu, fp, un
  ul = ["pixel", "", "", "", "", "nanometer", "micron", "mm", "cm", "meter", "km", "inch", "ft", "mi"]
  un = ul[un]

  vs = [1D, 1D, 1D]
  readu, fp, vs
  nc = 0L
  readu, fp, nc
  cid = bytarr(8)
  readu, fp, cid
  readu, fp, vol
  close, fp
  free_lun, fp, /FORCE
  return, vol
END
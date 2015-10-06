function [Mx, Ux] = DFastICA(Sx, unload)
    %% Checking some basic requirements of the data
    if nargin == 0,
        error ('You must supply the mixed data as input argument.');
    end
    dim = size(Sx);
    if length(dim) ~= 2,
        error ('Input data must have two dimensions.');
    end

    if any(any(isnan(Sx))),
        error ('Input data contains NaN''s.');
    end
    if nargin < 2 || ~isa(unload, 'logical')
        unload = false;
    end
    %% Checking and loading for FastICA shared lib
    global bFastICAReady;
    if isempty(bFastICAReady) || ~bFastICAReady,
        loadlibrary('FastICA', 'FastICA', 'thunkfilename', 'FastICA_Bridge');
        bFastICAReady = true;
    end
    %% Configure data type information to pass to shared lib
    dtype = 1;
    stype = cellstr(['uint8 ';'int16 ';'uint16';'int32 ';'uint32';'single';'double']);
    vtype = [1, 2, 12, 3, 13, 4, 5];
    for i = 1:length(stype)
        if isa(Sx, stype{i}),
            dtype = vtype(i);
            break
        end
    end
    % Data dimensions
    nc = dim(1);    % Number of components
	ns = dim(2);    % Number of samples
    % Create pointer storage for returned value, the mixing matrix (Mx)
    Rx = libpointer('doublePtr');
    %% Call FastICA shared lib
    if nargout == 1,
        %% Estimate Mx
        Rx = calllib('FastICA', 'Estimate', Sx, dtype, nc, ns);
    else
        %% Estimate Mx and unmix
        Ux = libpointer('uint16Ptr', Sx);
        Rx = calllib('FastICA', 'Unmix', [], Ux, 2, nc, ns);
        Ux = Ux.value;
    end
    %% Reform returned Mx coresponding to data layout (row/col base)
    if nc < ns,
        Rx.reshape(nc, nc);
    else
        Rx.reshape(ns, ns);
    end
    %% Copy result to output
    Mx = Rx.value;
    %% Unload shared lib after finish
    if unload
        DFastICA_Unload
    end
end
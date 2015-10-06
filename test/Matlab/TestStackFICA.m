%% Test FastICA with stack image
function TestStackFICA
    %% Load data of 3 channels from files
    c1 = MIV_Load('testdata\s1.miv');
    c2 = MIV_Load('testdata\s2.miv');
    [c3 dim] = MIV_Load('testdata\s3.miv');
    %% Form mixed signal buffer and display image
    ns = numel(c1);
    Sx = [reshape(c1, 1, ns);
          reshape(c2, 1, ns);
          reshape(c3, 1, ns)];
    % Display mixed signal image
    ShowFICASlice('Source Image', Sx, 5, dim, false);
    %% Estimate, unmix signals and display
    [Mx Ux] = DFastICA(Sx);
    % Show mixing matrix
    disp('Mx = ');disp(Mx);
    % Display unmixed image - return unmixed image is 16 bit that needs
    % to be rescaled to 8-bit for display
    ShowFICASlice('Unmixed Image', Ux, 5, dim, true);
end
%% Display test buffer image
function ShowFICASlice(name, stack, sid, dim, rescale)
    data = reshape(circshift(stack', [0 1]), dim(1), dim(2), dim(3), 3);
    data = reshape(data(:,:,sid,:), dim(1), dim(2), 3);
    figure('Name', name);
    if rescale
        data = uint8(255 * data / max(max(max(data))));
    end
    imshow(data);
end
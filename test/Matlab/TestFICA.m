%% Test FastICA
function TestFICA
    %% Load data of 3 channels from files
    c1 = MIV_Load('testdata\c1.miv');
    c2 = MIV_Load('testdata\c2.miv');
    c3 = MIV_Load('testdata\c3.miv');
    %% Form mixed signal buffer and display image
    ns = numel(c1);
    Sx = [reshape(c1, 1, ns);
          reshape(c2, 1, ns);
          reshape(c3, 1, ns)];
    % Display mixed signal image
    ShowFICA('Source Image', Sx);
    %% Estimate, unmix signals and display
    [Mx Ux] = DFastICA(Sx);
    % Show mixing matrix
    disp('Mx = ');disp(Mx);
    % Rescale unmixed signal to byte format for display
    Ux = uint8(255 * Ux / max(max(Ux)));
    % Display unmixed signal image
    ShowFICA('Unmixed Image', Ux);
end
%% Display test buffer image
function ShowFICA(name, data)
    figure('Name', name);
    imshow(reshape(circshift(data', [0 1]),512,512,3));
end
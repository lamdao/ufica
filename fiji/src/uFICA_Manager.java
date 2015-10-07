import ij.*;
import ij.gui.*;
import ij.io.OpenDialog;
import ij.io.SaveDialog;
import ij.plugin.frame.*;
import ij.process.LUT;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.*;

import javax.swing.BoxLayout;

import com.sun.jna.*;

/**
 *  This class manages the operations for
 *  Microscope Image Unmixing with FastICA engine
 *  
 *  @author Lam H. Dao <lam(dot)dao(at)nih(dot)gov>
 *                     <daohailam(at)yahoo(dot)com>
 */
public class uFICA_Manager extends PlugInFrame implements
	ActionListener, ItemListener, ImageListener, MouseListener {

	private static final long serialVersionUID = -707646654864137364L;
	private static uFICA_Manager instance;

	public interface DFastICA extends Library {
		Pointer Estimate(Pointer sources, int dtype, int nc, int ns);
		Pointer Unmix(Pointer ux, Pointer sources, int dtype, int nc, int ns);
		Pointer GetStatisticMax(Pointer sources, int nc, int ns);
		Pointer GetChannelMax();
		Pointer GetErrorMessage();
	}
	
	public class DataBuffer extends Memory {
		private boolean[] czeros;
		private long[] coffsets;
		private int nc;

		public DataBuffer(int nc, long ns) {
			super(nc * ns * Native.getNativeSize(Short.TYPE));
			coffsets = new long[this.nc = nc];
			czeros = new boolean [nc];
			for (int n = 0; n < nc; n++) {
				coffsets[n] = 2 * n;
				czeros[n] = true;
			}
		}
		
		public void add(int channel, short value) {
			setShort(coffsets[channel], value);
			coffsets[channel] += 2 * nc;
			czeros[channel] &= (value == 0);
		}
		
		// add byte-slice
		public void add(int channel, byte[] values) {
			for (int i = 0; i < values.length; i++) {
				add(channel, (short)(values[i] & 0xFF));
			}
		}

		// add short-slice
		public void add(int channel, short[] values) {
			for (int i = 0; i < values.length; i++) {
				add(channel, values[i]);
			}
		}
		
		// add RGB slice
		public void add(int[] values, byte cmask) {
			final int[] sx = {16, 8, 0};
			for (int i = 0; i < values.length; i++) {
				int v = values[i], cidx = 0;
				for (int c = 0; c < 3; c++) {
					if (!isOn(cmask, c)) continue;
					add(cidx, (short)((v >> sx[c]) & 0xFF));
					cidx = (cidx + 1) % nc;
				}
			}
		}

		public boolean valid() {
			if (!super.valid())
				return false;

			for (int i = 0; i < nc; i++)
				if (czeros[i]) {
					IJ.showMessage("Error",
							"Channel "+(i+1)+" is empty!\n"+
							"Please create a composite image using none-empty channels and redo.");
					return false;
				}

			return true;
		}

		public void reset() {
			for (int n = 0; n < nc; n++) {
				coffsets[n] = 2 * n;
			}
		}

		public short[] get(int channel, int psize) {
			long odx = coffsets[channel];
			short[] rx = new short[psize];
			for (int i = 0; i < psize; i++, odx += 2 * nc) {
				rx[i] = getShort(odx);
			}
			coffsets[channel] = odx;
			return rx;
		}
	}

	// Main Panel 
	protected java.awt.Panel panel;
	// List of unmixable images.
	protected java.awt.List wImList;
	protected java.awt.Label lblChannels;
	// Option to show mixing matrix after estimation
	protected Checkbox cbxShowMx;
	protected Vector<Checkbox> cbxChannels = new Vector<Checkbox>();
	// Buttons for user control.
	protected Button btnEstimate, btnUnmix, btnSaveMx, btnLoadMx;
	// List of image ids corresponding with images in unmixable list
	protected Vector<Integer> vIdList = new Vector<Integer>();
	// List of mixing matrices corresponding with image ids 
	protected HashMap<Integer, double[]> vMxList = new HashMap<Integer, double[]>();
	protected HashMap<Integer, Byte> vChList = new HashMap<Integer, Byte>();

	private static DFastICA fica = null;

	private static final LUT[] cluts;
	static {
		byte[] cc = new byte[256];
		byte[] zc = new byte[256];
		for (int i = 0; i < 256; i++) {
			cc[i] = (byte)i;
			zc[i] = 0;
		}
		cluts = new LUT[] {new LUT(cc, zc, zc),new LUT(zc, cc, zc),new LUT(zc, zc, cc)};
		
		String fext = (Platform.is64Bit() ? "64." : "32.") +
				 	(Platform.isWindows() ? "dll" : (Platform.isMac() ? "dylib" : "so"));
		String fdll = IJ.getDirectory("plugins").replace('\\','/')+"uFica/FastICA"+fext;
		try {
			fica = (DFastICA)Native.loadLibrary(fdll, DFastICA.class);
		}
		catch (UnsatisfiedLinkError ex) {
			IJ.showMessage("Error", ex.getMessage());
		}
	}
	//---------------------------------------------------------------------------------------------
	public uFICA_Manager() {
		this("\u00B5Fica - Microscope Image Unmixing with FastICA");
	}
	//---------------------------------------------------------------------------------------------
	public uFICA_Manager(String title) {
		super(title);
		instance = this;
		add(panel = createMainPanel());
		pack();
		setResizable(false);
		IJ.register(this.getClass());

		WindowManager.addWindow(this);
		ImagePlus.addImageListener(this);
		GUI.center(this);
		setVisible(true);
		updateButtonStatus();
	}
	//---------------------------------------------------------------------------------------------
	private int nc, ns, psize;
	private double[] mx = null;
	//---------------------------------------------------------------------------------------------
	private boolean isOn(byte cmask, int bit) {
		return (cmask & (1 << bit)) != 0;
	}
	//---------------------------------------------------------------------------------------------
	private Pointer getICAData() {
		Integer id = getCurrentId();
		if (null == id)
			return null;
		ImagePlus p = WindowManager.getImage(id);
		ImageStack s = p.getStack();
		int nbits = p.getBitDepth();
		if (nbits != 8 && nbits != 16 && nbits != 24) {
			IJ.showMessage("Error", "Unsupported data type.\n"+
									"Only 8-bit, 16-bit and RGB data type are supported.");
			return null;
		}

		byte cmask = vChList.get(id);
		int nt = p.isComposite() ? p.getNChannels() : 3;
		ns = (psize = p.getWidth() * p.getHeight()) * p.getNSlices() * p.getNFrames();
		nc = Integer.bitCount(cmask);

		DataBuffer data = new DataBuffer(nc, ns);
		if (p.isComposite()) {
			int cidx = 0;
			for (int z = 0; z < s.getSize(); z++) {
				if (!isOn(cmask, z % nt))
					continue;
				if (nbits == 8)
					data.add(cidx, (byte[])s.getPixels(z + 1));
				else
					data.add(cidx, (short[])s.getPixels(z + 1));

				cidx = (cidx + 1) % nc;
			}
		}
		else {	// RGB
			for (int z = 1; z <= s.getSize(); z++) {
				data.add((int[])s.getPixels(z), cmask);
			}
		}

		if (!data.valid())
			return null;

		data.reset();
		return data;
	}
	//---------------------------------------------------------------------------------------------
	private Integer getCurrentId() {
		int idx = wImList.getSelectedIndex();
		if (idx < 0)
			return null;
		return vIdList.get(idx);
	}
	//---------------------------------------------------------------------------------------------
	private double[] getMx() {
		Integer id = getCurrentId();
		return vMxList.get(id);
	}
	//---------------------------------------------------------------------------------------------
	private Pointer getMxPointer() {
		double[] mx = getMx();
		if (null == mx)
			return Pointer.NULL;
		Pointer p = new Memory(mx.length * Native.getNativeSize(Double.TYPE));
		p.write(0, mx, 0, mx.length);
		return p;
	}
	//---------------------------------------------------------------------------------------------
	private void ficaShowMx() {
		IJ.log("--- [ " + wImList.getSelectedItem() + " ] -----");
		IJ.log(" Mx = ");
		for (int n = 0; n < nc; n++) {
			String t = "";
			for (int i = 0; i < nc; i++)
				t += String.format("% 9.4f", mx[n * nc + i]);
			IJ.log(" "+t);
		}
	}
	//---------------------------------------------------------------------------------------------
	private void ficaEstimate() {
		Pointer dx = getICAData();
		if (dx == null)
			return;
		Pointer rx = fica.Estimate(dx, 2, nc, ns);
		if (rx == Pointer.NULL) {
			rx = fica.GetErrorMessage();
			IJ.showMessage("Error", rx.getString(0));
			return;
		}
		mx = rx.getDoubleArray(0, nc * nc);
		if (cbxShowMx.getState()) {
			ficaShowMx();
		}

		vMxList.put(getCurrentId(), mx.clone());
		btnSaveMx.setEnabled(true);
		Memory.purge();
	}
	//---------------------------------------------------------------------------------------------
	private void ficaUnmix() {
		//long t0 = System.currentTimeMillis();
		Pointer dx = getICAData();
		if (dx == null)
			return;
		Pointer ux = getMxPointer();
		Pointer rx = fica.Unmix(ux, dx, 2, nc, ns);
		if (rx == Pointer.NULL) {
			rx = fica.GetErrorMessage();
			IJ.showMessage("Error", rx.getString(0));
			return;
		}
		mx = rx.getDoubleArray(0, nc * nc);
		if (ux == Pointer.NULL) {
			vMxList.put(getCurrentId(), mx.clone());
			btnSaveMx.setEnabled(true);
			if (cbxShowMx.getState()) {
				ficaShowMx();
			}
		}
		// Create image from unmixed results
		ImagePlus p = WindowManager.getImage(getCurrentId());
		ImageStack s = new ImageStack(p.getWidth(), p.getHeight(), nc * p.getNSlices());
		for (int z = 0, n = 1; z < p.getNSlices(); z++) {
			for (int k = 0; k < nc; k++, n++) {
				short[] px = ((DataBuffer)dx).get(k, psize);
				s.setPixels(px, n);
			}
		}
		double[] smax = (fica.GetStatisticMax(dx, nc, ns)).getDoubleArray(0, nc);
		dx = null; Memory.purge(); System.gc();

		ImagePlus r = new ImagePlus(getICAName(p), s);
		r.setDimensions(nc, p.getNSlices(), p.getNFrames());
		r = new CompositeImage(r, CompositeImage.COMPOSITE);
		
		// Copy LUT from source image to ICA image
		boolean rgb = p.getBitDepth() == 24;
		LUT[] luts = rgb ? cluts : ((CompositeImage)p).getLuts();
		int nt = p.getNChannels() > 1 ? p.getNChannels() : 3;
		byte cmask = vChList.get(getCurrentId());
		for (int n = 0, c = 1; n < nt; n++) {
			if (!isOn(cmask, n)) continue;
			double lmax = smax[c-1];
			if (rgb) {
				if (lmax < 255) lmax = 255;
			} else {
				if (lmax < luts[n].max) lmax = luts[n].max;
			}
			luts[n].min = 0;
			luts[n].max = lmax;
			((CompositeImage)r).setChannelLut(luts[n], c++);
		}
		// Copy the calibration from the source
		r.getFileInfo().unit = p.getFileInfo().unit;
		r.setCalibration(p.getCalibration());
		//r.changes = true;
		r.show();
		//IJ.log("ET="+(0.001*(System.currentTimeMillis()-t0)));
	}
	//---------------------------------------------------------------------------------------------
	private String getICAName(ImagePlus p) {
		String ni = "";
		String title = p.getTitle();
		int dot = title.lastIndexOf('.');
		
		String name, ext;
		if (dot > 0) {
			name = title.substring(0, dot) + ".ICA.";
			ext = title.substring(dot + 1);
		} else {
			name = title + ".ICA.";
			ext = "tif";
		}
		int n = 1;
		while (true) {
			title = name + ni + ext;
			if (WindowManager.getImage(title) == null)
				break;
			ni = ++n + ".";
		}
		return title;
	}
	//---------------------------------------------------------------------------------------------
	// Save estimated mixing matrix to CSV file
	private void ficaSaveMx() throws Exception {
		String fn = wImList.getSelectedItem();
		if (fn.lastIndexOf('.') >= 0) {
			fn = fn.substring(0, fn.lastIndexOf('.')) + ".mx.";
		}
		SaveDialog sd = new SaveDialog("Save mixing matrix as CSV", fn, ".csv");
		String dir = sd.getDirectory();
		fn = sd.getFileName();
		if (null == dir || null == fn) {
			return;
		}

		dir = dir.replace('\\', '/');
		if (!dir.endsWith("/")) dir += "/";
		fn = dir + fn;

		double[] mx = getMx();
		int nc = (int)Math.round(Math.sqrt(mx.length));
		FileWriter fw = new FileWriter(fn);
		PrintWriter pw = new PrintWriter(fw);
		for (int i = 0; i < nc; i++) {
			for (int j = 0; j < nc; j++) {
				pw.print(mx[i*nc+j]);
				if (j < nc-1)
					pw.print(",");
			}
			pw.println();
		}
		pw.close();
		fw.close();
	}
	//---------------------------------------------------------------------------------------------
	// Load mixing matrix from CSV file and bind with selected image
	private void ficaLoadMx() throws Exception {
		OpenDialog od = new OpenDialog("Load mixing matrix from CSV", null, "*.csv");
		String dir = od.getDirectory();
		if (null == dir) return; // dialog was canceled
		dir = dir.replace('\\', '/'); // Windows safe
		if (!dir.endsWith("/")) dir += "/";
		java.util.Scanner scanner = new Scanner(new File(dir + od.getFileName()));
		Vector<Double> sx = new Vector<Double>();
		scanner.useDelimiter(",|;|\n|\t| ");
		while (scanner.hasNext()) {
			sx.add(Double.parseDouble(scanner.next()));
		}
		scanner.close();

		double[] mx = new double[sx.size()];
		for (int n = 0; n < mx.length; n++) {
			mx[n] = sx.get(n);
		}
		Integer id = getCurrentId();
		ImagePlus imp = WindowManager.getImage(id);
		int nc = imp.getBitDepth() == 24 ? 3 : imp.getNChannels();
		if (nc * nc < mx.length) {
			IJ.showMessage("Error",
					"Matrix size doesn't match with number of channels of selected image.");
			return;
		}
		vMxList.put(id, mx);
		btnSaveMx.setEnabled(true);
	}
	//---------------------------------------------------------------------------------------------
	// Enable/Disable functions base on status of selected item
	private void updateButtonStatus() {
		boolean ready = fica != null && wImList.getSelectedIndex() >= 0;
		Integer id = getCurrentId();
		btnEstimate.setEnabled(ready);
		btnUnmix.setEnabled(ready);
		btnSaveMx.setEnabled(ready && vMxList.get(id) != null);
		btnLoadMx.setEnabled(ready);

		lblChannels.setVisible(ready);
		if (ready) {
			ImagePlus p = WindowManager.getImage(id);
			int i, nc = p.getBitDepth() == 24 ? 3 : p.getNChannels();
			nc = Math.min(nc, cbxChannels.size());
			for (i = 0; i < nc; i++) {
				byte cstate = vChList.get(id);
				Checkbox cbx = cbxChannels.get(i);
				cbx.setState((cstate & (1 << i)) != 0);
				cbx.setVisible(true);
			}
			while (i < cbxChannels.size()) {
				cbxChannels.get(i).setVisible(false);
				i += 1;
			}
		} else {
			for (int i = 0; i < cbxChannels.size(); i++) {
				cbxChannels.get(i).setVisible(false);
			}
		}
	}
	//---------------------------------------------------------------------------------------------
	// Show/hide wait cursor and enable/disable input controls
	private void setBusy(boolean busy) {
		if (busy) {
			setCursor(new Cursor(Cursor.WAIT_CURSOR));
			panel.setEnabled(false);
		}
		else {
			setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			panel.setEnabled(true);
		}
	}
	//---------------------------------------------------------------------------------------------
	// Implementation of ActionListener interface.
	public void actionPerformed(ActionEvent e) {
		if (wImList == null) return;
		if (e.getSource() instanceof Button) {
			Button btn = (Button)e.getSource();

			try {
				setBusy(true);
				if (btn == btnEstimate) {
					ficaEstimate();
				}
				else if (btn == btnUnmix) {
					ficaUnmix();
				}
				else if (btn == btnSaveMx) {
					ficaSaveMx();
				}
				else if (btn == btnLoadMx) {
					ficaLoadMx();
				}
				setBusy(false);
			}
			catch (Exception ex) {
				IJ.handleException(ex);
			}
		}
	}
	//---------------------------------------------------------------------------------------------
	// Implementation of ItemListener interface
	public void itemStateChanged(ItemEvent e) {
		if (e.getSource() == wImList) {
			updateButtonStatus();
		} else {
			boolean off = e.getStateChange() == ItemEvent.DESELECTED;
			if (off) {
				int s = 0;
				for (int i = 0; i < cbxChannels.size(); i++) {
					if (cbxChannels.get(i).isVisible() && cbxChannels.get(i).getState()) {
						s += 1;
					}
				}
				if (s < 2) {
					//IJ.showMessage("Error", "At least 2 channels must be selected");
					((Checkbox)e.getSource()).setState(true);
					return;
				}
			}
			Integer id = getCurrentId();
			if (id != null) {
				vMxList.put(id, null);
				btnSaveMx.setEnabled(false);
			}
			int idx = cbxChannels.indexOf(e.getSource());
			byte cmsk = vChList.get(id);
			byte nmsk = (byte)(1 << idx);
			if (off)
				vChList.put(id, (byte)(cmsk & (~nmsk)));
			else
				vChList.put(id, (byte)(cmsk | nmsk));
		}
	}
	//---------------------------------------------------------------------------------------------
	// Implementation of ImageListener interface:
	// - update window list, if image is opened or closed
	public void imageOpened(ImagePlus imp) {
		String title = imp.getTitle();
		if (title.contains(".ICA")) return;
		int nc = imp.getBitDepth() == 24 ? 3 : imp.getNChannels();
		if (nc > 1) {
			Integer id = imp.getID();
			vMxList.put(id, null);
			vChList.put(id, (byte)(0x0F >> (4 - nc)));
			vIdList.add(id);
			wImList.add(title);
		}
	}
	//---------------------------------------------------------------------------------------------
	// - update window list, if image is opened or closed
	public void imageClosed(ImagePlus imp) {
		Integer id = imp.getID();
		int n = vIdList.indexOf(id);
		if (n < 0) return;
		vMxList.remove(id);
		vChList.remove(id);
		vIdList.remove(n);
		wImList.remove(n);

		updateButtonStatus();
	}
	//---------------------------------------------------------------------------------------------
	// - update window title, if image title is changed
	public void imageUpdated(ImagePlus imp) {
		String title = imp.getTitle();
		int idx = vIdList.indexOf(imp.getID());
		if (idx >= 0 && wImList.getItem(idx) != title) {
			wImList.replaceItem(title, idx);
		}
	}
	//---------------------------------------------------------------------------------------------
	// Implementation of MouseListener interface:
	public void mouseReleased(MouseEvent e) {}
	public void mousePressed(MouseEvent e) {}
	public void mouseExited(MouseEvent e) {}
	public void mouseEntered(MouseEvent e) {}
	public void mouseClicked(MouseEvent e) {
		IJ.showMessage("About",
				"\u00B5Fica - Microscope Image Unmixing with FastICA\n \n"+
				"Bleed-through/Cross-talk artifacts purification tool");
	}
	//---------------------------------------------------------------------------------------------
	// Create the list containing available images for unmixing
	private Panel createImageList() {
		Panel p = new Panel();
		BoxLayout l = new BoxLayout(p, BoxLayout.Y_AXIS);
		p.setLayout(l);
		p.add(new Label("Available multi-channel images:"));
		p.add(wImList = new java.awt.List(10, false));
		wImList.addItemListener(this);
		int[] ids = WindowManager.getIDList();
		if (ids != null)
		for (int id : ids) {
			imageOpened(WindowManager.getImage(id));
		}
		return p;
	}
	//---------------------------------------------------------------------------------------------
	private Panel createChannelList() {
		Panel p = new Panel(new FlowLayout(0, 4, 0));

		for (int c = 1; c <= 4; c++) {
			Checkbox cbx = new Checkbox(""+c, true);
			cbx.addItemListener(this);
			cbxChannels.add(cbx);
			p.add(cbx);
		}
		return p;
	}
	//---------------------------------------------------------------------------------------------
	// Create panel containing control buttons.
	protected Panel createControlPanel() {
		Panel p = new Panel(new GridLayout(2, 4, 2, 2));

		cbxShowMx = new Checkbox("Show mixing matrix", true);
		p.add(cbxShowMx);
		p.add(new Label());
		p.add(lblChannels = new Label("Use channels:", Label.RIGHT));
		p.add(createChannelList());

		btnEstimate = new Button("Estimate");
		btnEstimate.addActionListener(this);
		btnEstimate.setEnabled(false);
		p.add(btnEstimate);

		btnUnmix = new Button("Unmix");
		btnUnmix.addActionListener(this);
		btnUnmix.setEnabled(false);
		p.add(btnUnmix);

		btnSaveMx = new Button("Save mixing matrix");
		btnSaveMx.addActionListener(this);
		btnSaveMx.setEnabled(false);
		p.add(btnSaveMx);

		btnLoadMx = new Button("Load mixing matrix");
		btnLoadMx.addActionListener(this);
		btnLoadMx.setEnabled(false);
		p.add(btnLoadMx);

		return p;
	}
	//---------------------------------------------------------------------------------------------
	// Create the main GUI
	protected Panel createMainPanel() {
		Label copyright = new Label("(?)", Label.CENTER);
		copyright.setCursor(new Cursor(Cursor.HAND_CURSOR));
		copyright.addMouseListener(this);

		Panel p = new Panel(new BorderLayout(0, 3));
		p.add(createImageList(), BorderLayout.NORTH, 0);
		p.add(createControlPanel(), BorderLayout.CENTER, 1);
		p.add(copyright, BorderLayout.SOUTH, 2);
		return p;
	}
	//---------------------------------------------------------------------------------------------
	// Add padding space to default layout
	public Insets getInsets() {
		Insets i = super.getInsets();
		return new Insets(i.top + 10, i.left + 10, i.bottom + 10, i.right + 10);
	}
	//---------------------------------------------------------------------------------------------
	// Override parent windowClosing method to clean up synchronized resources on exit.
	public void windowClosing(WindowEvent e) {
		if (e.getSource() == this) {
			ImagePlus.removeImageListener(this);
			close();
		}
	}
	//---------------------------------------------------------------------------------------------
	// Override parent close method
	public void close() {
		super.close();
		instance = null;
	}
	//---------------------------------------------------------------------------------------------
	// Utility function to implement singleton model
	public static uFICA_Manager getInstance() {
		return instance;
	}
}

import java.awt.*;
import java.applet.Applet;
import java.awt.TextField;
import java.awt.event.*;
import java.applet.Applet;

public class HashCode extends Applet implements ActionListener, ItemListener, FocusListener {
	
	TextField hashCodeText;
	TextField hashCodeTextField;

	TextField integerText;
	TextField integerTextField;
	
	TextField hexText;
	TextField hexTextField;

	public void init() {
		
		hashCodeText = new TextField();
		hashCodeText.setEditable(false);
		hashCodeText.setText(new String("Hash Here:"));

		hashCodeTextField = new TextField();
		hashCodeTextField.setEditable(true);

		integerText = new TextField();
		integerText.setEditable(false);
		integerText.setText(new String("Integer Here:"));

		integerTextField = new TextField();
		integerTextField.setEditable(true);
		
		hexText = new TextField();
		hexText.setEditable(false);
		hexText.setText(new String("Hex Here:"));
		
		hexTextField = new TextField();
		hexTextField.setEditable(true);

        hashCodeTextField.addActionListener(this);
        integerTextField.addActionListener(this);
        hexTextField.addActionListener(this);

		hashCodeText.addFocusListener(this);
		integerText.addFocusListener(this);
		hexText.addFocusListener(this);

		setLayout(new java.awt.GridLayout(0,2));

		add(hashCodeText);
		add(hashCodeTextField);
		add(integerText);
		add(integerTextField);
		add(hexText);
		add(hexTextField);
	}

    public void itemStateChanged(ItemEvent e) {
    }

	static public long stringToLong(String s) {

		long i = s.charAt(0) * 256 * 256 * 256;
		i += s.charAt(1) * 256 * 256;
		i += s.charAt(2) * 256;
		i += s.charAt(3);
		return i;
	
	}

	static public String longToString(long l) {
		
		long subtractor = 256 * 256 * 256;

		String r = new String();

		short x = 1;

		while ( x < 5 ) {
			char thisOne = (char)(l / subtractor);
			r += thisOne;
			l -= thisOne * subtractor;
			x++;
			subtractor /= 256;
		}

		return r;
	}

	public void focusGained(FocusEvent e) {
		TextField tf = (TextField)e.getSource();
		tf.transferFocus();
	}

	public void focusLost(FocusEvent e) {
	}

    public void actionPerformed(ActionEvent e) {
		
		Object source = e.getSource();
		String label = e.getActionCommand();
		
		if ( source == hashCodeTextField ) {

			long l = stringToLong(label);
			integerTextField.setText(Long.toString(l));
			hexTextField.setText(Long.toHexString(l));
						
		} else if ( source == integerTextField ) {
			
			long l = Long.parseLong(label);
			hashCodeTextField.setText(longToString(l));
			hexTextField.setText(Long.toHexString(l));

		} else {

			long l = Long.parseLong(label, 16);			
			integerTextField.setText(Long.toString(l));
			hashCodeTextField.setText(longToString(l));

		}
    }


    public static void main(String[] args) {
	for (int i = 0; i < args.length; i++) {
	    String s = args[i];
	    // hashCode convert 'hash'
	    if (s.indexOf('\'') == 0 && s.length() == 6) {
		String ss = s.substring(1, 5);
		long l = HashCode.stringToLong(ss);
		System.out.println(s + "," + Long.toString(l) + ",0x" + Long.toHexString(l));
	    } else if (s.indexOf('0') == 0 && s.indexOf('x') == 1) {
		long l = Long.parseLong(s.substring(2), 16);
		String ss = HashCode.longToString(l);
		System.out.println("'" + ss + "'," + Long.toString(l) + ",0x" + Long.toHexString(l));
	    } else {
		long l = Long.parseLong(s);
		String ss = HashCode.longToString(l);
		System.out.println("'" + ss + "'," + Long.toString(l) + ",0x" + Long.toHexString(l));
	    }
	}
    }
}

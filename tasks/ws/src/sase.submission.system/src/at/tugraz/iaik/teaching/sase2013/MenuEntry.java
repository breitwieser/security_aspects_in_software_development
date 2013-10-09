/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.IOException;
import java.io.Writer;

/**
 * Menu entries pair a display name with an application resource to facilitate
 * automatic menu generation.
 * 
 * @author The SASE2013 Team
 */
public class MenuEntry {

	private final String name;
	private final AppResource resource;

	/**
	 * Creates a new <code>MenuEntry</code> from the given display name, and
	 * application resource.
	 * 
	 * @param name
	 *          the display name of the menu entry
	 * @param the
	 *          application resource this menu entry links to
	 */
	public MenuEntry(final String name, final AppResource resource) {
		this.name = name;
		this.resource = resource;
	}

	/**
	 * @return the display name of the menu entry
	 */
	public String getName() {
		return name;
	}

	/**
	 * @return the application resource this menu entry links to
	 */
	public AppResource getResource() {
		return resource;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((name == null) ? 0 : name.hashCode());
		result = prime * result + ((resource == null) ? 0 : resource.hashCode());
		return result;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		MenuEntry other = (MenuEntry) obj;
		if (name == null) {
			if (other.name != null)
				return false;
		} else if (!name.equals(other.name))
			return false;
		if (resource != other.resource)
			return false;
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "MenuEntry [name=" + name + ", resource=" + resource + "]";
	}

	/**
	 * Print the menu of given entries as a HTML list to the given output writer.
	 * This method is intended to be called from a JSP or servlet.
	 * 
	 * @param out
	 *          the output writer to write to
	 * @param entries
	 *          an array of menu entries to print
	 * @throws IOException
	 *           if an I/O problem occurs during writing to the output writer
	 */
	public static void printHtmlMenu(Writer out, MenuEntry[] entries)
	    throws IOException {
		for (MenuEntry entry : entries) {
			out.write("<li class='menu_entry'>\n");
			out.write("<a href='" + entry.getResource().getFullPath()
			    + "' class='menu_entry_ref'>\n");
			out.write(entry.getName() + "\n");
			out.write("</a></li>\n");
		}
	}
}

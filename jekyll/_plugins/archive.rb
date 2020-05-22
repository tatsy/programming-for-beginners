require "zip"

Jekyll::Hooks.register :site, :post_write do |site|
  site.data["archives"].each do |name, files|
    # Create archive directory
    destdir = File.join(site.dest, "archives")
    if not Dir.exists?(destdir)
      Dir.mkdir(destdir)
    end

    # Create ZIP archives
    Zip.continue_on_exists_proc = false
    Zip::File.open(File.join(destdir, name + ".zip"), Zip::File::CREATE) do |zipfile|
      files.each do |f|
        zipfile.add(File.basename(f), File.join(site.config["source"], f))
      end
    end
  end
end
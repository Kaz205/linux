int psi_show(struct seq_file *m, enum psi_res res)
{
	bool only_full = false;
	int full;

	for (full = 0; full < 2 - only_full; full++) {
		unsigned long avg[3] = { 0, };
		u64 total = 0;

		seq_printf(m, "%s avg10=%lu.%02lu avg60=%lu.%02lu avg300=%lu.%02lu total=%llu\n",
			   full || only_full ? "full" : "some",
			   LOAD_INT(avg[0]), LOAD_FRAC(avg[0]),
			   LOAD_INT(avg[1]), LOAD_FRAC(avg[1]),
			   LOAD_INT(avg[2]), LOAD_FRAC(avg[2]),
			   total);
	}

	return 0;
}

#ifdef CONFIG_PROC_FS
static int psi_io_show(struct seq_file *m, void *v)
{
	return psi_show(m, PSI_IO);
}

static int psi_memory_show(struct seq_file *m, void *v)
{
	return psi_show(m, PSI_MEM);
}

static int psi_cpu_show(struct seq_file *m, void *v)
{
	return psi_show(m, PSI_CPU);
}

static int psi_io_open(struct inode *inode, struct file *file)
{
	return single_open(file, psi_io_show, NULL);
}

static int psi_memory_open(struct inode *inode, struct file *file)
{
	return single_open(file, psi_memory_show, NULL);
}

static int psi_cpu_open(struct inode *inode, struct file *file)
{
	return single_open(file, psi_cpu_show, NULL);
}

static ssize_t psi_write(struct file *file, const char __user *user_buf,
			 size_t nbytes, enum psi_res res)
{
	if (!nbytes)
		return -EINVAL;

	return nbytes;
}

static ssize_t psi_io_write(struct file *file, const char __user *user_buf,
			    size_t nbytes, loff_t *ppos)
{
	return psi_write(file, user_buf, nbytes, PSI_IO);
}

static ssize_t psi_memory_write(struct file *file, const char __user *user_buf,
				size_t nbytes, loff_t *ppos)
{
	return psi_write(file, user_buf, nbytes, PSI_MEM);
}

static ssize_t psi_cpu_write(struct file *file, const char __user *user_buf,
			     size_t nbytes, loff_t *ppos)
{
	return psi_write(file, user_buf, nbytes, PSI_CPU);
}

static __poll_t psi_fop_poll(struct file *file, poll_table *wait)
{
	return DEFAULT_POLLMASK;
}

static int psi_fop_release(struct inode *inode, struct file *file)
{
	return single_release(inode, file);
}

static const struct proc_ops psi_io_proc_ops = {
	.proc_open	= psi_io_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_write	= psi_io_write,
	.proc_poll	= psi_fop_poll,
	.proc_release	= psi_fop_release,
};

static const struct proc_ops psi_memory_proc_ops = {
	.proc_open	= psi_memory_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_write	= psi_memory_write,
	.proc_poll	= psi_fop_poll,
	.proc_release	= psi_fop_release,
};

static const struct proc_ops psi_cpu_proc_ops = {
	.proc_open	= psi_cpu_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_write	= psi_cpu_write,
	.proc_poll	= psi_fop_poll,
	.proc_release	= psi_fop_release,
};

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
static int psi_irq_show(struct seq_file *m, void *v)
{
	return psi_show(m, PSI_IRQ);
}

static int psi_irq_open(struct inode *inode, struct file *file)
{
	return single_open(file, psi_irq_show, NULL);
}

static ssize_t psi_irq_write(struct file *file, const char __user *user_buf,
			     size_t nbytes, loff_t *ppos)
{
	return psi_write(file, user_buf, nbytes, PSI_IRQ);
}

static const struct proc_ops psi_irq_proc_ops = {
	.proc_open	= psi_irq_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_write	= psi_irq_write,
	.proc_poll	= psi_fop_poll,
	.proc_release	= psi_fop_release,
};
#endif

static int __init psi_proc_init(void)
{
	proc_mkdir("pressure", NULL);
	proc_create("pressure/io", 0666, NULL, &psi_io_proc_ops);
	proc_create("pressure/memory", 0666, NULL, &psi_memory_proc_ops);
	proc_create("pressure/cpu", 0666, NULL, &psi_cpu_proc_ops);
#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	proc_create("pressure/irq", 0666, NULL, &psi_irq_proc_ops);
#endif
	return 0;
}
module_init(psi_proc_init);

#endif /* CONFIG_PROC_FS */

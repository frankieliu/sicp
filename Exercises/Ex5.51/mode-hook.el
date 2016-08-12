(setq compilation-last-buffer nil)
(defun compile-again (pfx)
  """Run the same compile as the last time.

If there was no last time, or there is a prefix argument, this acts like
M-x compile.
"""
 (interactive "p")
 (if (and (eq pfx 1)
	  compilation-last-buffer)
     (progn
       (set-buffer compilation-last-buffer)
       (revert-buffer t t))
   (call-interactively 'compile)))
(defun my-c-mode-hook-config ()
  "For use with c-mode"
  (local-set-key (kbd "C-c c") 'compile-again))
(add-hook 'c-mode-hook 'my-c-mode-hook-config)
